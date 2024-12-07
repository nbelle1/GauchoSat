"""
AMU 3 Driver

This module contains classes, enums, and functions that provide a programmatic
interface to communicate and control the AMU.

Main components:
    - SweepType: Enumeration for the different types of sweeps that the AMU can perform.
    - DacGain: Enumeration for the possible gain settings for the DAC.
    - LED_PATTERN: Enumeration for LED patterns supported by the AMU.
    - AmuSweepConfiguration: Data class to represent configuration parameters for a sweep.
    - SweepMetadata: Data class to store metadata information for a sweep.
    - AMUDriver: Class for interfacing with the AMU, sending commands and reading responses.
    - find_com_port(): Function to detect the communication port of the AMU.
    - main(): Main execution method demonstrating various AMU operations.

Dependencies:
    - Requires the `serial` library for serial port communications.

Author: Scott Ireton
Company: Angstrom Designs, Inc.
Date: July 7, 2023
"""

import logging
from dataclasses import dataclass
from datetime import datetime
from enum import Enum
from time import perf_counter, sleep

from base_serial_driver import BaseSerialDriver

logger = logging.getLogger(__name__)


class SweepType(Enum):
    Linear = 0
    LogLinear = 1
    User1 = 2
    User2 = 3
    User3 = 4
    User4 = 5
    User5 = 6
    User6 = 7
    IV_NoiseTest = 10
    V_NoiseTest = 11
    I_NoiseTest = 12
    SimulatedSweep = 13


class DacGain(Enum):
    x1_4 = 0
    x1_2 = 1
    x1 = 2
    Auto = 3


class LED_PATTERN(Enum):
    OFF = 0
    WHITE_FLASH = 1
    QUICK_RGB_FADE = 2
    SLOW_RGB_FADE = 3
    RED_FLASH = 4
    GREEN_FLASH = 5
    BLUE_FLASH = 6


@dataclass
class AmuSweepConfiguration:
    sweep_type: SweepType
    number_of_points: int
    delay_ms: int
    ratio: int
    # deprecated "power"
    dac_gain: DacGain
    averages: int
    # reserved
    solar_constant: float
    cell_area: float


class InvalidAMUSweepConfigurationError(Exception):
    pass


@dataclass
class SweepMetadata:
    Voc: float
    Isc: float
    TSensorStart: float
    TSensorEnd: float
    FillFactor: float
    Efficiency: float
    VMax: float
    IMax: float
    PMax: float
    ADCTemperature: float
    Timestamp: int
    CRC32: int
    DateTime: str


class AMUDriver(BaseSerialDriver):

    _usb_vid = 9114
    _baudrate = 115200
    _timeout = 5  # seconds artibtrarily set to 5 seconds

    def __init__(self):
        super().__init__(vid=self._usb_vid, baudrate=self._baudrate, timeout=self._timeout)

        self._serial_number = ""
        self._firmware_version = ""
        self._sweep_config = None

    @property
    def device_name(self) -> str:
        """
        Returns the name of the device connected to the serial port.
        Should be overridden by subclasses.
        """
        return "AMU 3"

    @property
    def firmware_version(self) -> str:
        return self._firmware_version

    @property
    def serial_number(self) -> str:
        return self._serial_number

    @property
    def max_iv_points(self) -> int:
        return 80

    @property
    def min_iv_points(self) -> int:
        return 10

    @property
    def max_iv_averages(self) -> int:
        return 20

    @property
    def min_iv_averages(self) -> int:
        return 0

    def open(self):
        """Opens the serial connection."""
        super().open()

        # Get the IV sweep configuration from the Amu
        self._sweep_config = self.get_iv_sweep_config()

        # Read in metadata.
        self._metadata: dict[str, str] = {}
        self._read_metadata()

    def get_amu_metadata(self) -> dict[str, str]:
        return self._metadata

    # --- SCPI universal methods ---
    def CLS(self) -> None:
        """Clears the AMU's status byte."""
        command = "*CLS"
        self._query(command)

    def idn(self) -> str:
        """Returns the AMU's identification string.

        Returns:
            str: The AMU's identification string.
        """
        command = "*IDN?"
        response = self._query(command)
        return response

    def opc(self) -> bool:
        """Returns True if the AMU is ready to accept commands.

        Returns:
            bool: True if the AMU is ready to accept commands.
        """
        command = "*OPC?"
        response = self._query(command)
        return bool(int(response))

    # --- System methods ---
    def reset(self) -> None:
        command = "*RST"
        self._write(command)

    def preset(self) -> None:
        """Presets the status."""
        command = "STAT:PRES"
        self._write(command)

    def get_error(self) -> str:
        """Returns the AMU's next error message.

        Returns:
            str: The AMU's error message.
        """
        command = "SYST:ERR?"
        response = self._query(command)
        return response

    def get_error_count(self) -> int:
        """Returns the number of errors that have occurred.

        Returns:
            int: The number of errors that have occurred.
        """
        command = "SYST:ERR:COUN?"
        response = self._query(command)
        return int(response)

    def get_sweep_duration(self) -> float:
        """Returns the duration of the sweep in seconds.

        Returns:
            int: The duration of the sweep in seconds.
        """
        if not self._sweep_config:
            self._sweep_config = self.get_iv_sweep_config()

        # Sweeps are ~ 50 ms per point.
        per_point = 0.05  # TODO: Improve this based on actual amu firmware delay values.
        return per_point * self._sweep_config.number_of_points * self._sweep_config.averages

    def get_serial_number(self) -> str:
        command = "SYST:SER?"
        serial_number = self._query(command).replace('"', "")
        return serial_number

    def set_serial_number(self, serial_number: str) -> None:
        command = 'SYST:SER "{}"'.format(serial_number)
        self._write(command)

    def get_firmware_version(self) -> str:
        command = "SYST:FIRM?"
        return self._query(command).replace('"', "")

    def get_hardware_version(self) -> str:
        command = "SYST:HARD?"
        return self._query(command)

    # --- DUT methods ---
    def get_dut_serial_number(self) -> str:
        command = "DUT:SER?"
        return self._query(command).replace('"', "")

    # --- Measurement methods ---

    def get_iv_sweep_config(self) -> AmuSweepConfiguration:
        command = "SWEEP:CONF?"
        raw_response = self._query(command).split(",")

        self._sweep_config = AmuSweepConfiguration(
            sweep_type=SweepType(int(raw_response[0])),
            number_of_points=int(raw_response[1]),
            delay_ms=int(raw_response[2]),
            ratio=int(raw_response[3]),
            # 4 is deprecated "power"
            dac_gain=DacGain(int(raw_response[5])),
            averages=int(raw_response[6]),
            # 7 is reserved.
            solar_constant=float(raw_response[8]),
            cell_area=float(raw_response[9]),
        )

        # Update timeout.
        self.timeout = self.get_sweep_duration() + 2  # Add 2 seconds for good measure

        return self._sweep_config

    def set_sweep_config(self, sweep_config: AmuSweepConfiguration):
        # Error check.
        if sweep_config.number_of_points < self.min_iv_points:
            raise InvalidAMUSweepConfigurationError(f"Number of points must be at least {self.min_iv_points}.")
        if sweep_config.number_of_points > self.max_iv_points:
            raise InvalidAMUSweepConfigurationError(f"Number of points must be at most {self.max_iv_points}.")
        if sweep_config.averages > self.max_iv_averages:
            raise InvalidAMUSweepConfigurationError(f"Number of averages must be at most {self.max_iv_averages}.")
        if sweep_config.averages < self.min_iv_averages:
            raise InvalidAMUSweepConfigurationError(f"Number of averages must be at least {self.min_iv_averages}.")

        self._sweep_config = sweep_config

        command = "SWEEP:CONF:TYPE {}".format(sweep_config.sweep_type.value)
        self._write(command)
        command = "SWEEP:CONF:NUM {}".format(sweep_config.number_of_points)
        self._write(command)
        command = "SWEEP:CONF:DELAY {}".format(sweep_config.delay_ms)
        self._write(command)
        command = "SWEEP:CONF:RATIO {}".format(sweep_config.ratio)
        self._write(command)
        command = "SWEEP:CONF:GAIN {}".format(sweep_config.dac_gain.value)
        self._write(command)
        command = "SWEEP:CONF:AVER {}".format(sweep_config.averages)
        self._write(command)
        command = "SWEEP:CONF:AM0 {}".format(sweep_config.solar_constant)
        self._write(command)
        command = "SWEEP:CONF:AREA {}".format(sweep_config.cell_area)
        self._write(command)

        # Update timeout.
        self.timeout = self.get_sweep_duration() + 2  # Add 2 seconds for good measure

    def get_voc(self) -> tuple[float, float]:
        """Trigger and query a voc measurement.

        Returns:
            tuple[float, float]: Voc in volts, Temperature in degrees C
        """
        command = ":SWEEP:TRIG:VOC?"
        voc, temperature = map(float, self._query(command).split(","))
        return voc, temperature

    def get_isc(self) -> tuple[float, float]:
        """Trigger and query an isc measurement.

        Returns:
            tuple[float, float]: Isc in amps, Temperature in degrees C
        """
        command = ":SWEEP:TRIG:ISC?"
        isc, temperature = map(float, self._query(command).split(","))
        return isc, temperature

    def get_iv(self) -> tuple[list[int], list[float], list[float]]:
        """Trigger and query data for a solar cell IV sweep.

        Returns:
            tuple[list[int], list[float], list[float]]: Timestamps, voltage, current
        """
        command = ":SWEEP:TRIG:USB"
        raw_result = self._query(command)
        lines = raw_result.split("\r\n")

        timestamps = []
        voltage = []
        current = []

        for line in lines:
            data = line.split("\t")
            timestamps.append(int(data[0]))
            voltage.append(float(data[1]))
            current.append(float(data[2]))

        return timestamps, voltage, current

    def get_sweep_metadata(self) -> SweepMetadata:
        command = "SWEEP:META?"
        response = self._query(command).split(",")
        metadata = SweepMetadata(
            Voc=float(response[0]),
            Isc=float(response[1]),
            TSensorStart=float(response[2]),
            TSensorEnd=float(response[3]),
            FillFactor=float(response[4]),
            Efficiency=float(response[5]),
            VMax=float(response[6]),
            IMax=float(response[7]),
            PMax=float(response[8]),
            ADCTemperature=float(response[9]),
            Timestamp=int(response[10]),
            CRC32=int(response[11]),
            DateTime=datetime.now().strftime("%Y-%m-%d %H:%M:%S"),  # Get the date and time when querying the amu for IV data
        )
        return metadata

    def get_twi_address(self):
        command = "SYST:TWI:ADD?"
        response = self._query(command)
        address = int(response)
        return address

    def set_twi_address(self, address: int):
        if not self._is_uint8(address):
            raise ValueError("TWI address must be between 0 and 127.")
        command = "SYST:TWI:ADD {}".format(address)
        self._write(command)

    def sweep_set_dac_voltage(self, voltage: float):
        command = "DAC:VOLT {:.3f}".format(voltage)
        self._write(command)

    # --- Raw ADC methods ---
    def get_adc_temperature_raw(self) -> float:
        command = "MEASure:ADC:TSENSOR0?"
        response = self._query(command)
        return float(response)

    # --- LED methods ---
    def set_led_pattern(self, pattern: LED_PATTERN) -> None:
        # TODO: Need to add pattern.
        command = f"SYST:LED:PAT {pattern.value}"
        self._write(command)

    def commit_memory(self) -> None:
        command = "MEM:COMM"
        self._write(command)

    # --- private read/write methods ---
    def _write(self, command: str) -> None:
        """Writes the command to the AMU.

        Args:
            command (str): The command to write.
        """
        if not self.is_connected:
            self.open()
        cmd = (command.strip() + "\r\n").encode()
        self.ser.write(cmd)

    def _read(self) -> str:
        """Reads the response from the AMU.

        Returns:
            str: The response from the AMU decoded to UTF-8.
        """
        if not self.ser.is_open:
            self.open()

        # Wait for a response.
        start = perf_counter()
        while not self.ser.in_waiting:
            if self.timeout and (perf_counter() - start > self.timeout):
                raise TimeoutError("Timed out waiting for response from AMU.")

        # Now that we have a response, let's read the entire thing!
        response = bytes()
        while len(response) == 0 or response[-1] != 10 or self.ser.in_waiting:  # 10 == '\n'
            response += self.ser.read(self.ser.in_waiting)
            sleep(0.0005)

        return response.decode("utf-8").strip()

    def _query(self, command: str) -> str:
        """Writes the query then waits for the response and decodes it to utf-8.

        Args:
            command (str): The query.

        Returns:
            str: The utf-8 decoded response.
        """
        self._write(command)
        return self._read()

    def _is_uint8(self, value: int) -> bool:
        """Returns True if the value is a uint8.

        Args:
            value (int): The value to check.

        Returns:
            bool: True if the value is a uint8.
        """
        return value >= 0 and value <= 255

    def _read_metadata(self):
        self._serial_number = self.get_serial_number()
        self._firmware_version = self.get_firmware_version()
        self._metadata = {
            "AMU IDN": self.idn(),
            "AMU SN": self._serial_number,
            "AMU HW VER": self.get_hardware_version(),
            "AMU FW VER": self._firmware_version,
            "AMU TWI ADDR": f"0x{self.get_twi_address():x}",
            "AMU DUT SN": self.get_dut_serial_number(),
        }
        logger.info(f"Connected to AMU: {self._metadata['AMU SN']}")


def main():
    amu = AMUDriver()
    amu.set_led_pattern(LED_PATTERN.BLUE_FLASH)
    amu.set_led_pattern(LED_PATTERN.SLOW_RGB_FADE)
    print("Connected to AMU at: {}".format(amu.port))

    idn = amu.idn()
    print(f"IDN: {idn}")

    sn = amu.get_serial_number()
    print(f"Serial Number: {sn}")

    dut_sn = amu.get_dut_serial_number()
    print(f"DUT Serial Number: {dut_sn}")

    twi_address = amu.get_twi_address()
    print(f"TWI Address: 0x{twi_address:02X}")

    voc, temperature = amu.get_voc()
    print(f"Voc: {voc} V, Temperature: {temperature} C")

    isc, temperature = amu.get_isc()
    print(f"Isc: {isc} A, Temperature: {temperature} C")

    config = amu.get_iv_sweep_config()
    print(f"IV Sweep Config: {config}")

    timestamps, voltage, current = amu.get_iv()
    print(f"IV Sweep Data: {timestamps}, {voltage}, {current}")

    metadata = amu.get_sweep_metadata()
    print(f"Sweep Metadata: {metadata}")

    # amu.set_serial_number("040000210018")
    # amu.set_twi_address(0x20)
    # amu.commit_memory()

    # start = perf_counter()
    # while True:
    #     elapsed = perf_counter() - start
    #     isc = amu.get_isc()[0] * 1000  # mA
    #     voc = amu.get_voc()[0]  # V
    #     print(f"{elapsed:6.1f}\t{isc:6.3f}\t{voc:5.3f}")
    #     # print(f"ISC = {amu.get_isc()[0]*1000:6.3f} mA     VOC = {amu.get_voc()[0]:5.3f} V")
    #     sleep(1)


if __name__ == "__main__":
    main()