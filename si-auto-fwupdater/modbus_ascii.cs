using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;

namespace si_auto_fwupdater
{
    class modbus_ascii
    {
        SerialPort sp = new SerialPort();

        public string setPort(string ComPort)
        {
            if (!sp.IsOpen)
                sp.PortName = ComPort;

            return sp.PortName;
        }
        public void openPort()
        {
            sp.BaudRate = 57600;
            sp.StopBits = StopBits.One;
            sp.Parity = Parity.Even;
            sp.DataBits = 7;
            sp.ReadTimeout = 1000;
            sp.WriteTimeout = 1000;
            sp.NewLine = "\r\n";
            sp.Open();
        }

        public void closePort()
        {
            try
            {
                if (sp.IsOpen)
                    sp.Close();
            }
            catch (Exception ex)
            {

            }

        }


        byte modbus_LRC(byte[] buf, int startPos, int len)
        {
            byte lrc = 0;
            while (len-- > 0)
            {
                lrc += buf[startPos++];
            }

            return (byte)-lrc;
        }

        public static byte[] StringToByteArray(String hex)
        {
            int NumberChars = hex.Length;
            byte[] bytes = new byte[NumberChars / 2];
            for (int i = 0; i < NumberChars; i += 2)
                bytes[i / 2] = Convert.ToByte(hex.Substring(i, 2), 16);
            return bytes;
        }

        byte[] modbusASCII_readReg(Byte slaveAddr, UInt16 regAddr, UInt16 regCnt)
        {
            if (regCnt > 125)
                throw new ArgumentOutOfRangeException(nameof(regCnt), "Too many registers requested");

            byte[] modbus_buf = new byte[7];

            modbus_buf[0] = slaveAddr;  // slave address
            modbus_buf[1] = 4;          // function code (4)
            modbus_buf[2] = (byte)(regAddr >> 8);   // regAddr MSB
            modbus_buf[3] = (byte)(regAddr & 0xff); // regAddr LSB
            modbus_buf[4] = (byte)(regCnt >> 8);   // regCnt MSB
            modbus_buf[5] = (byte)(regCnt & 0xff); // regCnt LSB
            modbus_buf[6] = modbus_LRC(modbus_buf, 1, 5);   // LRC

            string modbusStr = ":" + BitConverter.ToString(modbus_buf, 0, 7).Replace("-", "");



            sp.WriteLine(modbusStr);

            sp.ReadTimeout = 1000;

            modbusStr = sp.ReadLine();
            return StringToByteArray(modbusStr.Substring(modbusStr.IndexOf(':') + 1));

        }
        byte[] modbusASCII_readHoldReg(Byte slaveAddr, UInt16 regAddr, UInt16 regCnt)
        {
            if (regCnt > 125)
                throw new ArgumentOutOfRangeException(nameof(regCnt), "Too many registers requested");

            byte[] modbus_buf = new byte[7];

            modbus_buf[0] = slaveAddr;  // slave address
            modbus_buf[1] = 3;          // function code (3)
            modbus_buf[2] = (byte)(regAddr >> 8);   // regAddr MSB
            modbus_buf[3] = (byte)(regAddr & 0xff); // regAddr LSB
            modbus_buf[4] = (byte)(regCnt >> 8);   // regCnt MSB
            modbus_buf[5] = (byte)(regCnt & 0xff); // regCnt LSB
            modbus_buf[6] = modbus_LRC(modbus_buf, 1, 5);   // LRC

            string modbusStr = ":" + BitConverter.ToString(modbus_buf, 0, 7).Replace("-", "");



            sp.WriteLine(modbusStr);

            sp.ReadTimeout = 1000;

            modbusStr = sp.ReadLine();
            return StringToByteArray(modbusStr.Substring(modbusStr.IndexOf(':') + 1));

        }

        bool modbusASCII_writeHoldRegs(Byte slaveAddr, UInt16 regAddr, UInt16 regCnt, byte[] regData)
        {
            if (regCnt > 125)
                throw new ArgumentOutOfRangeException(nameof(regCnt), "Too many registers requested");

            if (regCnt * 2 != regData.Length)
                throw new ArgumentOutOfRangeException(nameof(regData.Length), "Length mismatch");



            // Reverse bytes (LE -> BE)
            if (BitConverter.IsLittleEndian)
            {
                for (int i = 0; i < regCnt; i++)
                {
                    Array.Reverse(regData, i * 2, 2);
                }
            }




            byte[] modbus_buf = new byte[8 + regData.Length];

            modbus_buf[0] = slaveAddr;  // slave address
            modbus_buf[1] = 16;          // function code (16)
            modbus_buf[2] = (byte)(regAddr >> 8);   // regAddr MSB
            modbus_buf[3] = (byte)(regAddr & 0xff); // regAddr LSB
            modbus_buf[4] = (byte)(regCnt >> 8);   // regCnt MSB
            modbus_buf[5] = (byte)(regCnt & 0xff); // regCnt LSB
            modbus_buf[6] = (byte)(regData.Length); // byte count

            int modbus_buf_idx = 7;


            regData.CopyTo(modbus_buf, modbus_buf_idx);

            modbus_buf_idx += regData.Length;

            modbus_buf[modbus_buf_idx] = modbus_LRC(modbus_buf, 1, modbus_buf_idx - 1);   // LRC

            string modbusStr = ":" + BitConverter.ToString(modbus_buf, 0, modbus_buf.Length).Replace("-", "");



            sp.WriteLine(modbusStr);

            sp.ReadTimeout = 1000;

            modbusStr = sp.ReadLine();




            byte[] modbus_resp = StringToByteArray(modbusStr.Substring(modbusStr.IndexOf(':') + 1));

            if (modbus_resp.Length != 7)
            {
                // Length wrong
                return false;
            }


            byte LRC = modbus_LRC(modbus_resp, 1, 5);


            if (LRC != modbus_resp[6])
            {
                // LRC wrong
                return false;
            }

            for (int i = 0; i < 6; i++)
            {
                if (modbus_resp[i] != modbus_buf[i])
                {
                    // wrong reply
                    return false;
                }

            }



            return true;
        }






        // bootloader functions



        public bool modbusASCII_bootReset(Byte slaveAddr)
        {

            byte[] modbus_buf = new byte[255];

            modbus_buf[0] = slaveAddr;  // slave address
            modbus_buf[1] = 0x6c;       // function code (6c)
            modbus_buf[2] = 1;          // sub function code (0)
            modbus_buf[3] = modbus_LRC(modbus_buf, 1, 2);   // LRC

            string modbusStr = ":" + BitConverter.ToString(modbus_buf, 0, 4).Replace("-", "");



            sp.WriteLine(modbusStr);

            sp.ReadTimeout = 1000;

            modbusStr = sp.ReadLine();




            byte[] modbus_resp = StringToByteArray(modbusStr.Substring(modbusStr.IndexOf(':') + 1));

            if (modbus_resp.Length != 4)
            {
                // Length wrong
                return false;
            }


            byte LRC = modbus_LRC(modbus_resp, 1, 2);


            if (LRC != modbus_resp[3])
            {
                // LRC wrong
                return false;
            }

            for (int i = 0; i < 3; i++)
            {
                if (modbus_resp[i] != modbus_buf[i])
                {
                    // wrong reply
                    return false;
                }

            }


            return true;

        }





        public byte[] modbusASCII_bootCheckComm(Byte slaveAddr)
        {

            byte[] modbus_buf = new byte[255];

            modbus_buf[0] = slaveAddr;  // slave address
            modbus_buf[1] = 0x6c;       // function code (6c)
            modbus_buf[2] = 0;          // sub function code (0)
            modbus_buf[3] = modbus_LRC(modbus_buf, 1, 2);   // LRC

            string modbusStr = ":" + BitConverter.ToString(modbus_buf, 0, 4).Replace("-", "");



            sp.WriteLine(modbusStr);

            sp.ReadTimeout = 1000;

            modbusStr = sp.ReadLine();




            byte[] modbus_resp = StringToByteArray(modbusStr.Substring(modbusStr.IndexOf(':') + 1));

            if (modbus_resp.Length != 8)
            {
                // Length wrong
                return null;
            }


            byte LRC = modbus_LRC(modbus_resp, 1, 6);


            if (LRC != modbus_resp[7])
            {
                // LRC wrong
                return null;
            }

            for (int i = 0; i < 3; i++)
            {
                if (modbus_resp[i] != modbus_buf[i])
                {
                    // wrong reply
                    return null;
                }

            }

            byte[] buf = new byte[4];

            Buffer.BlockCopy(modbus_resp, 3, buf, 0, 4);

            return buf;

        }


        public byte[] modbusASCII_bootReadFlash(Byte slaveAddr, UInt32 flashAddr, Byte flashCnt)
        {
            if (flashCnt > 246)
                throw new ArgumentOutOfRangeException(nameof(flashCnt), "Too many bytes requested");

            byte[] modbus_buf = new byte[255];

            modbus_buf[0] = slaveAddr;  // slave address
            modbus_buf[1] = 0x6c;       // function code (6c)
            modbus_buf[2] = 2;          // sub function code (2)
            modbus_buf[3] = (byte)(flashAddr >> 16);    // flashAddr MSB
            modbus_buf[4] = (byte)(flashAddr >> 8);
            modbus_buf[5] = (byte)(flashAddr & 0xff);   // flashAddr LSB
            modbus_buf[6] = flashCnt;                   // flashCnt
            modbus_buf[7] = modbus_LRC(modbus_buf, 1, 6);   // LRC

            string modbusStr = ":" + BitConverter.ToString(modbus_buf, 0, 8).Replace("-", "");



            sp.WriteLine(modbusStr);

            sp.ReadTimeout = 1000;

            modbusStr = sp.ReadLine();




            byte[] modbus_resp = StringToByteArray(modbusStr.Substring(modbusStr.IndexOf(':') + 1));

            if (modbus_resp.Length != flashCnt + 8)
            {
                // Length wrong
                return null;
            }


            byte LRC = modbus_LRC(modbus_resp, 1, 134);


            if (LRC != modbus_resp[flashCnt + 7])
            {
                // LRC wrong
                return null;
            }

            for (int i = 0; i < 7; i++)
            {
                if (modbus_resp[i] != modbus_buf[i])
                {
                    // wrong reply
                    return null;
                }

            }

            byte[] fwBuf = new byte[flashCnt];

            Buffer.BlockCopy(modbus_resp, 7, fwBuf, 0, flashCnt);

            return fwBuf;

        }



        public bool modbusASCII_bootEraseFlash(Byte slaveAddr, Byte secNum)
        {

            byte[] modbus_buf = new byte[255];

            modbus_buf[0] = slaveAddr;  // slave address
            modbus_buf[1] = 0x6c;       // function code (6c)
            modbus_buf[2] = 3;          // sub function code (3)
            modbus_buf[3] = 0;          // secNum MSB
            modbus_buf[4] = secNum;     // secNum LSB
            modbus_buf[5] = 0xCC;       // Key1
            modbus_buf[6] = 0x33;       // Key2
            modbus_buf[7] = modbus_LRC(modbus_buf, 1, 6);   // LRC

            string modbusStr = ":" + BitConverter.ToString(modbus_buf, 0, 8).Replace("-", "");



            sp.WriteLine(modbusStr);

            sp.ReadTimeout = 1000;

            modbusStr = sp.ReadLine();




            byte[] modbus_resp = StringToByteArray(modbusStr.Substring(modbusStr.IndexOf(':') + 1));

            if (modbus_resp.Length !=  8)
            {
                // Length wrong
                return false;
            }


            byte LRC = modbus_LRC(modbus_resp, 1, 6);


            if (LRC != modbus_resp[7])
            {
                // LRC wrong
                return false;
            }

            for (int i = 0; i < 7; i++)
            {
                if (modbus_resp[i] != modbus_buf[i])
                {
                    // wrong reply
                    return false;
                }

            }

            return true;

        }

        public bool modbusASCII_bootWriteFlash(Byte slaveAddr, UInt32 flashAddr, Byte flashCnt, byte[] flashData)
        {


            if (flashCnt > 244)
                throw new ArgumentOutOfRangeException(nameof(flashCnt), "Too many bytes requested");

            byte[] modbus_buf = new byte[255];






            modbus_buf[0] = slaveAddr;  // slave address
            modbus_buf[1] = 0x6c;       // function code (6c)
            modbus_buf[2] = 4;          // sub function code (4)
            modbus_buf[3] = (byte)(flashAddr >> 16);    // flashAddr MSB
            modbus_buf[4] = (byte)(flashAddr >> 8);
            modbus_buf[5] = (byte)(flashAddr & 0xff);   // flashAddr LSB
            modbus_buf[6] = flashCnt;                   // flashCnt
            modbus_buf[7] = 0x55;       // Key1
            modbus_buf[8] = 0xAA;       // Key2
            Buffer.BlockCopy(flashData, 0, modbus_buf, 9, flashCnt);
            modbus_buf[flashCnt + 9] = modbus_LRC(modbus_buf, 1, flashCnt + 8);   // LRC
            string modbusStr = ":" + BitConverter.ToString(modbus_buf, 0, flashCnt + 10).Replace("-", "");



            sp.WriteLine(modbusStr);

            sp.ReadTimeout = 1000;

            modbusStr = sp.ReadLine();




            byte[] modbus_resp = StringToByteArray(modbusStr.Substring(modbusStr.IndexOf(':') + 1));

            if (modbus_resp.Length != 10)
            {
                // Length wrong
                return false;
            }


            byte LRC = modbus_LRC(modbus_resp, 1, 8);


            if (LRC != modbus_resp[9])
            {
                // LRC wrong
                return false;
            }

            for (int i = 0; i < 9; i++)
            {
                if (modbus_resp[i] != modbus_buf[i])
                {
                    // wrong reply
                    return false;
                }

            }

            return true;

        }



    }
}
