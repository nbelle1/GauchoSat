#include <Arduino.h>
#include <Wire.h>

#define IVSWEEP_POINTS 40      // Must match setting in AMU.
#define AMU_TWI_ADDRESS 0x0F // Must match AMU

#define TWI_BUFFER_LEN 32

#define AMU_REG_CMD 0x00
#define AMU_REG_DATA_PTR_TIMESTAMP 0xF0
#define AMU_REG_DATA_PTR_VOLTAGE 0xF1
#define AMU_REG_DATA_PTR_CURRENT 0xF2
#define AMU_REG_DATA_PTR_SWEEP_META 0xF6
#define AMU_REG_TRANSFER_PTR 0xFE

#define CMD_SWEEP_TRIG_SWEEP 0x0142
#define CMD_SWEEP_TRIG_ISC 0x0143
#define CMD_SWEEP_TRIG_VOC 0x0144

#define AMU_TWI_TRANSFER_READ 1
#define AMU_TWI_TRANSFER_WRITE 0

#define CMD_RW_BIT 7
#define CMD_READ (1 << CMD_RW_BIT)
#define CMD_WRITE (0 << CMD_RW_BIT)

#define AMU_TRANSFER_REG_SIZE (IVSWEEP_POINTS * 8) // sizeof(float) * 2

// Global variables.
static volatile uint8_t amu_transfer_reg[AMU_TRANSFER_REG_SIZE];

// Type definitions.
typedef struct
{
  float voc;
  float isc;
  float tsensor_start;
  float tsensor_end;
  float ff;
  float eff;
  float vmax;
  float imax;
  float pmax;
  float adc;
  uint32_t timestamp;
  uint32_t crc;
} ivsweep_meta_t;

typedef struct
{
  float measurement;
  float temperature;
} amu_meas_t;

// Function prototypes.
int amu_wire_transfer(uint8_t address, uint8_t reg, uint8_t *data, size_t len, uint8_t read);
void measure_voc();
void measure_isc();
void measure_iv_curve();
int8_t amu_dev_send_command(uint8_t address, uint16_t command);

template <typename T>
void read_twi_reg(uint8_t address, uint8_t reg, T *data, size_t len);

template <typename T>
T query(uint16_t command);

// Main program. Uncomment to test
// void setup()
// {
//   Serial.begin(115200);
//   Wire.begin();
//   Wire.setClock(400000);
//   Wire.setTimeout(1000);
//   delay(1000);
//   pinMode(18, INPUT_PULLUP);
//   pinMode(19, INPUT_PULLUP);

//   Serial.println("Hi, welcome to AMU example.");
// }

// void loop()
// {
//   if (Serial.available() > 0)
//   {
//     char received = Serial.read();
//     if (received == 'i')
//     {
//       Serial.println("Measuring Isc...");
//       measure_isc();
//     }
//     else if (received == 'v')
//     {
//       Serial.println("Measuring Voc...");
//       measure_voc();
//     }
//     else if (received == 's')
//     {
//       Serial.println("Measuring IV Curve...");
//       measure_iv_curve();
//     }
//   }
// }

// Function definitions.
int amu_wire_transfer(uint8_t address, uint8_t reg, uint8_t *data, size_t len, uint8_t read)
{
  uint8_t packetNum = 0;
  if (read)
  {
    if (len > 0)
    {
      Wire.beginTransmission(address);
      Wire.write(reg);
      Wire.endTransmission();
      while (len > TWI_BUFFER_LEN)
      {
        Wire.requestFrom(address, (uint8_t)TWI_BUFFER_LEN, false);
        Wire.readBytes(&data[packetNum * TWI_BUFFER_LEN], TWI_BUFFER_LEN);
        packetNum++;
        len -= TWI_BUFFER_LEN;
      }
      Wire.requestFrom(address, (uint8_t)len);
      Wire.readBytes(&data[packetNum * TWI_BUFFER_LEN], len);
    }
    else
    {
      Wire.beginTransmission(address);
      return Wire.endTransmission();
    }
  }
  else
  {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(data, len);
    Wire.endTransmission();
  }

  return 0;
}

int8_t amu_dev_send_command(uint8_t address, uint16_t command)
{
  return amu_wire_transfer(address, (uint8_t)AMU_REG_CMD, (uint8_t *)&command, 1, AMU_TWI_TRANSFER_WRITE);
}

template <typename T>
void read_twi_reg(uint8_t address, uint8_t reg, T *data, size_t len)
{
  amu_wire_transfer(address, reg, (uint8_t *)data, len, AMU_TWI_TRANSFER_READ);
}

template <typename T>
T query(uint16_t command)
{
  T *data = (T *)amu_transfer_reg;
  amu_dev_send_command(AMU_TWI_ADDRESS, (command | CMD_READ));
  delay(100);
  amu_wire_transfer(AMU_TWI_ADDRESS, (uint8_t)AMU_REG_TRANSFER_PTR, (uint8_t *)amu_transfer_reg, sizeof(T), AMU_TWI_TRANSFER_READ);
  return *data;
}

int16_t compress_temperature(float temperature)
{
  return (int16_t)(temperature * 100);
}

int16_t compress_current(float current)
{
  return (int16_t)(current * 1e4);
}

int16_t compress_voltage(float voltage)
{
  return (int16_t)(voltage * 1e3);
}

void measure_voc()
{
  // Trigger Voc.
  amu_meas_t measurement = query<amu_meas_t>((uint16_t)CMD_SWEEP_TRIG_VOC);

  // Print the Voc data.
  Serial.print("\n\n");
  Serial.print("Voc: ");
  Serial.println(measurement.measurement, 6);
  Serial.print("Temperature: ");
  Serial.println(measurement.temperature, 6);
}

void measure_isc()
{
  // Trigger Isc.
  amu_meas_t measurement = query<amu_meas_t>((uint16_t)CMD_SWEEP_TRIG_ISC);

  // Print the Voc data.
  Serial.print("\n\n");
  Serial.print("Isc: ");
  Serial.println(measurement.measurement, 6);
  Serial.print("Temperature: ");
  Serial.println(measurement.temperature, 6);
}

void measure_iv_curve()
{
  amu_dev_send_command(AMU_TWI_ADDRESS, (uint16_t)CMD_SWEEP_TRIG_SWEEP);
  delay(1500); // Wait for sweep to finish.

  // Read the sweep data.
  uint32_t timestamp[IVSWEEP_POINTS];
  float voltage[IVSWEEP_POINTS];
  float current[IVSWEEP_POINTS];
  read_twi_reg<uint32_t>(AMU_TWI_ADDRESS, AMU_REG_DATA_PTR_TIMESTAMP, timestamp, sizeof(uint32_t) * IVSWEEP_POINTS);
  read_twi_reg<float>(AMU_TWI_ADDRESS, AMU_REG_DATA_PTR_VOLTAGE, voltage, sizeof(float) * IVSWEEP_POINTS);
  read_twi_reg<float>(AMU_TWI_ADDRESS, AMU_REG_DATA_PTR_CURRENT, current, sizeof(float) * IVSWEEP_POINTS);

  ivsweep_meta_t sweep_meta;
  read_twi_reg<ivsweep_meta_t>(AMU_TWI_ADDRESS, AMU_REG_DATA_PTR_SWEEP_META, &sweep_meta, sizeof(ivsweep_meta_t));

  Serial.println("Metadata:");
  Serial.print("Voc: ");
  Serial.println(sweep_meta.voc);
  Serial.print("Isc: ");
  Serial.println(sweep_meta.isc);
  Serial.print("Tsensor Start: ");
  Serial.println(sweep_meta.tsensor_start);
  Serial.print("Tsensor End: ");
  Serial.println(sweep_meta.tsensor_end);
  Serial.print("FF: ");
  Serial.println(sweep_meta.ff);
  Serial.print("Eff: ");
  Serial.println(sweep_meta.eff);
  Serial.print("Vmax: ");
  Serial.println(sweep_meta.vmax);
  Serial.print("Imax: ");
  Serial.println(sweep_meta.imax);
  Serial.print("Pmax: ");
  Serial.println(sweep_meta.pmax);
  Serial.print("ADC: ");
  Serial.println(sweep_meta.adc);
  Serial.print("Timestamp: ");
  Serial.println(sweep_meta.timestamp);
  Serial.print("CRC: ");
  Serial.println(sweep_meta.crc);
  Serial.println();
  Serial.flush();
  Serial.println("IV Curve: ");
  for (int i = 0; i < IVSWEEP_POINTS; i++)
  {
    Serial.print(timestamp[i], 6);
    Serial.print("\t");
    Serial.print(voltage[i], 12);
    Serial.print("\t");
    Serial.print(current[i], 12);
    Serial.print("\n");
  }
  Serial.println();
}