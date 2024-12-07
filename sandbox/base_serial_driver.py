import logging
from time import perf_counter, sleep
from typing import Optional

import serial
import serial.tools.list_ports
from serial import SerialException

logger = logging.getLogger(__name__)


class ComPortNotFoundError(Exception):
    """Exception raised when the COM port can't be discovered."""

    def __init__(self, vid, device_name: str = ""):
        message: str
        if device_name != "":
            message = f'COM port for device "{device_name}" with VID {vid} not found.'
        else:
            message = f"COM port with VID {vid} not found."
        message += " Please check that the device is connected."
        super().__init__(message)


class BaseSerialDriver:
    def __init__(self, vid: Optional[int] = None, com_port: Optional[str] = None, baudrate: int = 115200, timeout: Optional[float] = None):
        """
        Initializes a new instance of the BaseSerialDriver class. Either vid or com_port must be specified.

        Args:
            vid (int, optional): The vendor ID of the device to connect to.
            com_port (str, optional): The COM port to use for the serial connection.
            baudrate (int, optional): The baud rate for the serial connection. Default is 115200.
            timeout (float, optional): The timeout for the serial connection. Default is None.

        Raises:
            ValueError: If neither vid nor com_port is specified.
        """
        if self.device_name == "":
            logger.warning("device_name property not overridden in BaseSerialDriver subclass. Please override it.")

        if vid is None and com_port is None:
            raise ValueError("Either vid or com_port must be specified.")

        self._vid = vid
        self._ser = None
        self._port = com_port
        self._baudrate = baudrate
        self._timeout = timeout

    @property
    def port(self) -> str:
        """Serial port name."""
        return self._port if self._port else "None"

    @property
    def timeout(self) -> float | None:
        """Serial timeout in seconds."""
        return self._timeout

    @timeout.setter
    def timeout(self, value: float | None):
        """
        Sets the timeout value for the serial driver.

        Args:
            value (float | None): The timeout value in seconds. Pass `None` for no timeout.

        Returns:
            None
        """
        self._timeout = value
        if self.ser.is_open:
            self.ser.timeout = value

    @property
    def device_name(self) -> str:
        """
        Returns the name of the device connected to the serial port.
        Should be overridden by subclasses.
        """
        return ""

    @property
    def ser(self) -> serial.Serial:
        """Get the ser attribute."""
        if self._ser is None:
            raise serial.SerialException("ser property accessed before serial connection was opened.")
        return self._ser

    @property
    def is_connected(self) -> bool:
        """Returns True if the serial connection is open, otherwise False."""
        return self._ser is not None and self._ser.is_open

    def close(self):
        """Closes the serial connection."""
        if self._ser is not None and self._ser.is_open:
            self._ser.close()

    def reset_serial_connection(self):
        """Special function used if the USB is disconnected.
        When this happens, the variables self.ser.is_open is still True, and
        self.ser is NOT none. So we need to reset those. We can't call close in this case
        because although python thinks self.ser exists, there is actually no connection."""
        if self._ser is not None:
            self._ser.is_open = False
            self._ser = None

    def open(self):
        """
        Opens the serial connection.

        If the serial connection is already open, this method does nothing.
        If a port is not provided, it will try to find the port based on the VID.

        Raises:
            ComPortNotFoundError: If the specified hardware ID is not found.
            ValueError: If neither vid nor com_port is specified.
        """
        # Make sure it's not already open.
        if self._ser and self._ser.is_open:
            return

        # Check if a port is already provided.
        if not self._port:
            if not self._vid:
                raise ValueError("Either vid or com_port must be specified.")
            self._port = self._find_com_port(self._vid)

        start = perf_counter()
        while self._ser is None or not self._ser.is_open:
            try:
                self._ser = serial.Serial(port=self._port, baudrate=self._baudrate, timeout=self._timeout)
            except SerialException:
                # Sometimes if the device was JUST plugged in, it hasn't had time to boot
                # so you get a serial exception. Let's retry until timeout.
                if perf_counter() - start > (self.timeout if self.timeout else 5):
                    raise
                sleep(0.1)

    def reconnect(self, timeout=5):
        """Closes and reopens the serial connection."""
        self.close()
        start = perf_counter()
        while perf_counter() - start < timeout:
            try:
                self.open()
                return
            except serial.SerialException:
                pass
        self.open()

    def _find_com_port(self, vid: int):
        """
        Finds the COM port associated with the specified hardware ID.

        Args:
            vid (int): The vendor ID to search for.

        Returns:
            str: The COM port associated with the specified hardware ID.

        Raises:
            ComPortNotFoundError: If the specified hardware ID is not found.
        """
        ports = serial.tools.list_ports.comports()
        for i in range(len(ports)):
            if ports[i].vid == vid:  # adjust the substring search accordingly
                return ports[i].device
        raise ComPortNotFoundError(vid, self.device_name)

    def __del__(self):
        # check if we have a self.ser.
        # If we do, close it.
        if self._ser is not None and self._ser.is_open:
            self._ser.close()

    def __enter__(self):
        """Context manager entry point."""
        self.open()
        return self

    def __exit__(self, *args, **kwargs):
        """Context manager exit point."""
        self.close()


if __name__ == "__main__":
    inst = BaseSerialDriver(0x1A86)  # SPE6103 power supply.