import serial
import argparse
import time
import logging
import pyvjoy # Windows apenas
handshake = False 

class MyControllerMap:
    def __init__(self):
        self.button = {'A': 1, 'B': 2, 'C': 3, 'D': 4, 'E' : 5, 'F' : 6}


class SerialControllerInterface:

    # Protocolo
    # byte 1 -> Botão 1 (estado - Apertado 1 ou não 0)
    # byte 2 -> EOP - End of Packet -> valor reservado 'X'

    def __init__(self, port, baudrate):
        self.ser = serial.Serial(port, baudrate=baudrate)
        self.mapping = MyControllerMap()
        self.j = pyvjoy.VJoyDevice(1)
        self.incoming = '0'

    def update(self):
        ## Sync protocol
        global handshake
        while self.incoming != b'X':
            self.incoming = self.ser.read()
            logging.debug("Received INCOMING: {}".format(self.incoming))

        #para cada parte do struct enviado, deve-se fazer 1 read
        data_id = self.ser.read()
        data_status = self.ser.read()
        logging.debug("Received DATA_ID: {}".format(data_id))
        logging.debug("Received DATA_status: {}".format(data_status))


        if (handshake is False):
            #rotina para handhsahe
            print("esperando handhshake")
            if (data_id == b'5'):
                logging.debug("recebi handhsake")
                self.ser.write(b'A')
                logging.debug("Received INCOMING: {}".format(b'A'))
                handshake = True

        if(handshake is True):
            if data_id == b'1':
                if (data_status == b'1'): 
                    self.j.set_button(self.mapping.button['A'], 1)

                elif data_status == b'0':
                    self.j.set_button(self.mapping.button['A'], 0)

            if data_id == b'2':
                if (data_status == b'1'): 
                    self.j.set_button(self.mapping.button['B'], 1)

                elif data_status == b'0':
                    self.j.set_button(self.mapping.button['B'], 0)

            if data_id == b'3':
                if (data_status == b'1'): 
                    self.j.set_button(self.mapping.button['C'], 1)

                elif data_status == b'0':
                    self.j.set_button(self.mapping.button['C'], 0)
                    
            if data_id == b'4':
                if (data_status == b'1'): 
                    self.j.set_button(self.mapping.button['D'], 1)

                elif data_status == b'0':
                    self.j.set_button(self.mapping.button['D'], 0)
            else:
                if(data_id == b'U'): #up
                    self.j.set_button(self.mapping.button['E'], 1)
                    #soltando
                    self.j.set_button(self.mapping.button['E'], 0)

                if(data_id == b'D'): #down
                    self.j.set_button(self.mapping.button['F'], 1)
                    #soltando
                    self.j.set_button(self.mapping.button['F'], 0)

        self.incoming = self.ser.read()


class DummyControllerInterface:
    def __init__(self):
        self.mapping = MyControllerMap()
        self.j = pyvjoy.VJoyDevice(1)

    def update(self):
        self.j.set_button(self.mapping.button['A'], 1)
        time.sleep(0.1)
        self.j.set_button(self.mapping.button['A'], 0)
        logging.info("[Dummy] Pressed A button")
        time.sleep(1)


if __name__ == '__main__':
    interfaces = ['dummy', 'serial']
    argparse = argparse.ArgumentParser()
    argparse.add_argument('serial_port', type=str)
    argparse.add_argument('-b', '--baudrate', type=int, default=9600)
    argparse.add_argument('-c', '--controller_interface', type=str, default='serial', choices=interfaces)
    argparse.add_argument('-d', '--debug', default=False, action='store_true')
    args = argparse.parse_args()
    if args.debug:
        logging.basicConfig(level=logging.DEBUG)

    print("Connection to {} using {} interface ({})".format(args.serial_port, args.controller_interface, args.baudrate))
    if args.controller_interface == 'dummy':
        controller = DummyControllerInterface()
    else:
        controller = SerialControllerInterface(port=args.serial_port, baudrate=args.baudrate)

    while True:
        controller.update()
