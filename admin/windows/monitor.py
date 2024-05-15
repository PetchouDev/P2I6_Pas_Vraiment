import time
from threading import Thread
from serial import Serial
from multipledispatch import dispatch

import tkinter as tk
from tkinter import ttk
from tkinter import scrolledtext

class Monitor(Thread, Serial):
    def __init__(self, port, baudrate, parent=None):
        Thread.__init__(self)
        Serial.__init__(self, port, baudrate)
        self.daemon = True
        self.parent = parent
        self.running = False

    def run(self):
        print("Monitor thread started")
        while self.running:
            # if there is data to read
            if self.in_waiting:
                data = self.read(self.in_waiting)
                # si le buffer n'est pas vide
                if data:
                    self.parent.last_received = time.time()
                self.parent.log(data)
        print("Monitor thread stopped")

    def start(self):
        self.running = True
        super().start()

    @dispatch(str)
    def send(self, data):
        self.write(data.encode('utf-8'))

    @dispatch(bytes)
    def send(self, data):
        self.write(data)


class MonitorSupervisor(Thread):
    def __init__(self, parent=None):
        Thread.__init__(self)
        self.daemon = True
        self.parent = parent
        self.running = False

    def run(self):
        print("Monitor supervisor thread started")
        while self.running:
            print(f"paquet reçu il y a {time.time() - self.parent.last_received:.3f} secondes")
            if time.time() - self.parent.last_received > .4 and  self.parent.idle == False:
                self.parent.reload_monitor(None)
            time.sleep(1)

    def start(self):
        self.running = True
        super().start()



class SerialMonitorApp(tk.Tk):
    def __init__(self, port, baudrate):
        self.port = port
        self.baudrate = baudrate
        self.idle = False
        self.last_received = time.time() + 1 # add one to avoid the first reload
        super().__init__()
        self.logfield = scrolledtext.ScrolledText(self) 
        self.logfield.grid(row=0, column=0, columnspan=7, sticky="ew", padx=5, pady=5)

        # entry to send data
        self.send_entry = ttk.Entry(self)
        self.send_entry.grid(row=1, column=0, columnspan=7, sticky="ew", padx=5, pady=5)
        self.send_entry.bind("<Return>", self.send_data)

        # check button for debug
        self.check_button = ttk.Button(self, text="Check")
        self.check_button.grid(row=2, column=1, columnspan=1, sticky="ew", padx=5, pady=5)
        self.check_button.bind("<Button-1>", self.health_check)

        # start button
        self.start_button = ttk.Button(self, text="Start")
        self.start_button.grid(row=2, column=2, columnspan=1, sticky="ew", padx=5, pady=5)
        self.start_button.bind("<Button-1>", self.start_monitor)

        # stop button
        self.stop_button = ttk.Button(self, text="Stop")
        self.stop_button.grid(row=2, column=3, columnspan=1, sticky="ew", padx=5, pady=5)
        self.stop_button.bind("<Button-1>", self.stop_monitor)

        # state button
        self.state_button = ttk.Button(self, text="State")
        self.state_button.grid(row=2, column=4, columnspan=1, sticky="ew", padx=5, pady=5)
        self.state_button.bind("<Button-1>", self.state_monitor)

        # reload button
        self.reload_button = ttk.Button(self, text="Reload")
        self.reload_button.grid(row=2, column=5, columnspan=1, sticky="ew", padx=5, pady=5)
        self.reload_button.bind("<Button-1>", self.reload_monitor)
        

        # bind window close event
        self.protocol("WM_DELETE_WINDOW", self.on_closing)

        self.serial_monitor = Monitor(port, baudrate, self)
        self.serial_monitor.start()

        self.monitor_supervisor = MonitorSupervisor(self)
        self.monitor_supervisor.start()

        self.serial_monitor.send("state")

    def start_monitor(self, event):
        self.idle = False
        self.serial_monitor.send("start")
    
    def stop_monitor(self, event):
        self.idle = True
        self.serial_monitor.send("stop")

    def state_monitor(self, event):
        self.serial_monitor.send("state")
    

    def reload_monitor(self, event):
        print(self.idle, event, self.idle == True, event == None)
        if self.idle == True and event == None:
            print('Idle')
            return
        print('Reloading')
        # kill the thread
        self.serial_monitor.running = False
        self.serial_monitor.join()

        # libérer le port
        self.serial_monitor.close()

        # supprimer le thread
        del self.serial_monitor

        # reset last received
        self.last_received = time.time() + 1

        # create a new thread
        self.serial_monitor = Monitor(self.port, self.baudrate, self)
        self.serial_monitor.start()


    def send_data(self, event):
        data = self.send_entry.get()
        self.serial_monitor.send(data)
        self.send_entry.delete(0, tk.END)

    def log(self, data):
        try:
            if isinstance(data, bytes):
                data = data.decode('utf-8')
            self.logfield.insert(tk.END, data)
            self.logfield.see(tk.END)
        except:
            pass
        

    def on_closing(self):
        # kill monitor supervisor
        self.monitor_supervisor.running = False
        self.monitor_supervisor.join()
        del self.monitor_supervisor

        # kill monitor
        self.serial_monitor.running = False
        self.serial_monitor.join()
        self.serial_monitor.close()
        del self.serial_monitor

        # destroy the window
        self.quit()
        self.destroy()

    def health_check(self, event):
        res = f"""Health check
        port: {self.serial_monitor.port}
        baudrate: {self.serial_monitor.baudrate}
        running: {self.serial_monitor.running}
        open: {self.serial_monitor.is_open}"""
        print(res)
        self.log(res)