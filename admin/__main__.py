from windows.setupWindow import Setup
from windows.monitor import SerialMonitorApp

if __name__ == "__main__":
    app = Setup()
    conf = app.get_data()
    if "error" in conf:
        print(conf)
    else:
        monitor = SerialMonitorApp(conf["port"], conf["baudrate"])
        monitor.mainloop()