import tkinter as tk
from tkinter import ttk

from time import sleep

from serial.tools import list_ports
from threading import Thread

class Worker(Thread):
    def __init__(self, parent=None):
        super().__init__()
        self.daemon = True
        self.parent = parent
        self.running = False

    def run(self):
        if self.parent:
            while self.running:
                self.parent.check_baudrate_value()
                self.parent.check_port_value()
                self.parent.update_connect_button()
                sleep(0.03)
        print("Worker thread stopped")

    def start(self) -> None:
        self.running = True
        return super().start()

class Setup(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Serial Monitor Setup")
        self.geometry("500x180")
        self.minsize(500, 180)

        # state of the entries to check if they can be valid
        self.valid_port = True
        self.valid_baudrate = True

        # worker thread to check the values of the entries
        self.worker = Worker(self)

        # declare the variables to store the data to be returned
        self.ports = [f"{port.name} - {port.description}" for port in self.get_available_ports()]
        self.selected_port = tk.StringVar()
        self.selected_port.set(self.ports[0])

        self.selected_port_2 = tk.StringVar()
        self.selected_port_2.set(self.ports[0])

        tmp = []

        for port in self.ports.copy():
            if "Arduino" in port:
                tmp = [port] + tmp
            else:
                tmp.append(port)
        
        self.ports = tmp
        del tmp


        self.baudrate = tk.StringVar()

        self.title = tk.Label(self, text="Serial Monitor Setup", font=("Arial", 20))
        self.title.grid(row=0, column=0, columnspan=7)

        self.port_label = tk.Label(self, text="Serail port 1")
        self.port_label.grid(row=1, column=1)
        
        self.port_combobox = ttk.Combobox(self, textvariable=self.selected_port, values=self.ports)
        self.port_combobox.grid(row=1, column=2, columnspan=3, sticky="ew", padx=5, pady=5)
        self.columnconfigure(2, weight=1)
        self.port_combobox.current(0)
        #self.port_combobox.bind("<<ComboboxSelected>>", self.check_baudrate_value)

        self.refresh_button = tk.Button(self, text="Refresh", command=self.refresh_ports)
        self.refresh_button.grid(row=1, column=5, sticky="ew", padx=5, pady=5)

        """self.port_label_2 = tk.Label(self, text="Serial port 2")
        self.port_label_2.grid(row=2, column=1)
        
        self.port_combobox_2 = ttk.Combobox(self, textvariable=self.selected_port_2, values=self.ports)
        self.port_combobox_2.grid(row=2, column=2, columnspan=3, sticky="ew", padx=5, pady=5)

        self.refresh_button_2 = tk.Button(self, text="Refresh", command=self.refresh_ports)
        self.refresh_button_2.grid(row=2, column=5, sticky="ew", padx=5, pady=5)"""



        self.baudrate_label = tk.Label(self, text="Baudrate")
        self.baudrate_label.grid(row=3, column=1, sticky="ew")

        self.baudrate_entry = tk.Entry(self, textvariable=self.baudrate)
        self.baudrate.set("19200")
        self.baudrate_entry.grid(row=3, column=2, columnspan=2, sticky="ew", padx=5, pady=5)
        #self.baudrate_entry.bind("<Key>", self.check_baudrate_value)

        self.connect_button = tk.Button(self, text="Connect", command=self.exit)
        self.connect_button.config(state=tk.DISABLED)
        self.connect_button.grid(row=4, column=2, columnspan=3, sticky="ew", padx=5, pady=5)

        # attach window close event to custom function
        self.protocol("WM_DELETE_WINDOW", self.quit)

        # start the worker thread
        self.worker.start()
        

    def check_baudrate_value(self, event=None):
        try:
            int(self.baudrate.get())
            self.valid_baudrate = True
        except:
            self.valid_baudrate = False
            print('Invalid baudrate')
        

    def check_port_value(self, event=None):
        if self.selected_port.get() in self.ports:
            self.valid_port = True
        else:
            self.valid_port = False
        

    def update_connect_button(self):
        if self.valid_port and self.valid_baudrate:
            self.connect_button.config(state=tk.NORMAL)
        else:
            self.connect_button.config(state=tk.DISABLED)

    def set_default_baudrate(self, event=None):
        self.baudrate.set("9600")

    def get_available_ports(self):
        return sorted(list_ports.comports())
        

    def refresh_ports(self):
        self.ports = [f"{port.name} - {port.description}" for port in self.get_available_ports()]
        self.port_combobox['values'] = self.ports

    def get_data(self):
        self.mainloop()

        data = {
            "port": self.selected_port.get().split(" - ")[0],
            #"port_2": self.selected_port.get().split(" - ")[0],
            "baudrate": int(self.baudrate.get())
        }

        if isinstance(data["baudrate"], int):
            return data
        else:
            return {"error": "Invalid data", "data": data}
    
    def exit(self):
        """
        Stop the worker thread and close the window
        """
        self.worker.running = False
        self.worker.join()
        super().quit()
        super().destroy()




if __name__ == "__main__":
    app = Setup()
    print(app.get_data())
