import time
from threading import Thread
import os, sys  # pour installer les dépendances

import tkinter as tk
from tkinter import ttk
from tkinter import scrolledtext

def import_or_install(package):
    """
    Importe un package ou l'installe s'il n'est pas disponible.

    Args:
        package (str): Le nom du package à importer ou installer.
    """
    try:
        __import__(package)
        del sys.modules[package]
    except ImportError:
        os.system(f"{sys.executable} -m pip install {package}")

# Importer ou installer les dépendances nécessaires
import_or_install("pyserial")
from serial import Serial

import_or_install("multipledispatch")
from multipledispatch import dispatch

class Monitor(Thread, Serial):
    """
    Classe représentant un moniteur en série qui écoute les données reçues sur un port série
    dans un thread séparé.

    Attributes:
        parent (SerialMonitorApp): La fenêtre principale pour accéder aux éléments de l'interface graphique.
        running (bool): État du thread.
    """
    def __init__(self, port, baudrate, parent=None):
        Thread.__init__(self)
        Serial.__init__(self, port, baudrate)
        self.daemon = True
        self.parent = parent
        self.running = False

    def run(self):
        """
        Tâche du thread : écouter les données sur le port série et les afficher.
        """
        print("Monitor thread started")
        while self.running:
            if self.in_waiting:
                data = self.read(self.in_waiting)
                if data:
                    self.parent.last_received = time.time()
                    self.parent.log(data)
        print("Monitor thread stopped")

    def start(self):
        """
        Démarrer le thread.
        """
        self.running = True
        super().start()

    @dispatch(str)
    def send(self, data):
        """
        Envoyer un message depuis une chaîne de caractères.

        Args:
            data (str): Le message à envoyer.
        """
        self.write(data.encode('utf-8'))

    @dispatch(bytes)
    def send(self, data):
        """
        Envoyer un message depuis un tableau d'octets.

        Args:
            data (bytes): Le message à envoyer.
        """
        self.write(data)

class MonitorSupervisor(Thread):
    """
    Classe supervisant le moniteur en série pour redémarrer le thread en cas de problème.

    Attributes:
        parent (SerialMonitorApp): La fenêtre principale pour accéder aux éléments de l'interface graphique.
        running (bool): État du thread.
    """
    def __init__(self, parent=None):
        Thread.__init__(self)
        self.daemon = True
        self.parent = parent
        self.running = False

    def run(self):
        """
        Tâche du thread : vérifier l'état du moniteur et redémarrer si nécessaire.
        """
        print("Monitor supervisor thread started")
        while self.running:
            print(f"paquet reçu il y a {time.time() - self.parent.last_received:.3f} secondes")
            if time.time() - self.parent.last_received > .4 and self.parent.idle == False:
                self.parent.reload_monitor(None)
            time.sleep(1)

    def start(self):
        """
        Démarrer le thread.
        """
        self.running = True
        super().start()

class SerialMonitorApp(tk.Tk):
    """
    Application tkinter pour surveiller et interagir avec un port série.

    Attributes:
        port (str): Le port série à utiliser.
        baudrate (int): Le débit en bauds du port série.
        idle (bool): État d'inactivité.
        last_received (float): Heure de la dernière réception de données.
    """
    def __init__(self, port, baudrate):
        self.port = port
        self.baudrate = baudrate
        self.idle = False
        self.last_received = time.time() + 1  # add one to avoid the first reload
        super().__init__()

        # Champ de texte pour afficher les logs
        self.logfield = scrolledtext.ScrolledText(self)
        self.logfield.grid(row=0, column=0, columnspan=7, sticky="ew", padx=5, pady=5)

        # Entrée pour envoyer des données
        self.send_entry = ttk.Entry(self)
        self.send_entry.grid(row=1, column=0, columnspan=7, sticky="ew", padx=5, pady=5)
        self.send_entry.bind("<Return>", self.send_data)

        # Boutons de contrôle
        self.check_button = ttk.Button(self, text="Check")
        self.check_button.grid(row=2, column=1, columnspan=1, sticky="ew", padx=5, pady=5)
        self.check_button.bind("<Button-1>", self.health_check)

        # Bouton pour (re)démarrer le moniteur
        self.start_button = ttk.Button(self, text="Start")
        self.start_button.grid(row=2, column=2, columnspan=1, sticky="ew", padx=5, pady=5)
        self.start_button.bind("<Button-1>", self.start_monitor)

        # Bouton pour arrêter le moniteur
        self.stop_button = ttk.Button(self, text="Stop")
        self.stop_button.grid(row=2, column=3, columnspan=1, sticky="ew", padx=5, pady=5)
        self.stop_button.bind("<Button-1>", self.stop_monitor)

        # Bouton pour afficher l'état du moniteur
        self.state_button = ttk.Button(self, text="State")
        self.state_button.grid(row=2, column=4, columnspan=1, sticky="ew", padx=5, pady=5)
        self.state_button.bind("<Button-1>", self.state_monitor)


        # Bouton pour redémarrer le moniteur
        self.reload_button = ttk.Button(self, text="Reload")
        self.reload_button.grid(row=2, column=5, columnspan=1, sticky="ew", padx=5, pady=5)
        self.reload_button.bind("<Button-1>", self.reload_monitor)

        # Attacher une méthode à la fermeture de la fenêtre
        self.protocol("WM_DELETE_WINDOW", self.on_closing)

        # Initialiser les moniteurs
        self.serial_monitor = Monitor(port, baudrate, self)
        self.serial_monitor.start()

        self.monitor_supervisor = MonitorSupervisor(self)
        self.monitor_supervisor.start()

        self.serial_monitor.send("state")

    def start_monitor(self, event):
        """
        Démarrer le moniteur en série.

        Args:
            event (tk.Event): L'événement déclencheur.
        """
        self.idle = False
        self.serial_monitor.send("start")

    def stop_monitor(self, event):
        """
        Arrêter le moniteur en série.

        Args:
            event (tk.Event): L'événement déclencheur.
        """
        self.idle = True
        self.serial_monitor.send("stop")

    def state_monitor(self, event):
        """
        Envoyer une commande d'état au moniteur en série.

        Args:
            event (tk.Event): L'événement déclencheur.
        """
        self.serial_monitor.send("state")

    def reload_monitor(self, event):
        """
        Redémarrer le moniteur en série.

        Args:
            event (tk.Event): L'événement déclencheur ou None.
        """
        if self.idle == True and event == None:
            print('Idle')
            return
        print('Reloading')
        self.serial_monitor.running = False
        self.serial_monitor.join()
        self.serial_monitor.close()
        del self.serial_monitor
        self.last_received = time.time() + 1
        self.serial_monitor = Monitor(self.port, self.baudrate, self)
        self.serial_monitor.start()

    def send_data(self, event):
        """
        Envoyer les données entrées par l'utilisateur.

        Args:
            event (tk.Event): L'événement déclencheur.
        """
        data = self.send_entry.get()
        self.serial_monitor.send(data)
        self.send_entry.delete(0, tk.END)

    def log(self, data):
        """
        Afficher les données reçues dans le champ de texte.

        Args:
            data (str ou bytes): Les données à afficher.
        """
        try:
            if isinstance(data, bytes):
                data = data.decode('utf-8')
            self.logfield.insert(tk.END, data)
            self.logfield.see(tk.END)
        except:
            pass

    def on_closing(self):
        """
        Gestion de la fermeture de la fenêtre.
        """
        self.serial_monitor.send("stop") # Arrêter le moniteur en série
        
        self.monitor_supervisor.running = False # Arrêter le superviseur
        self.monitor_supervisor.join() # Attendre la fin du superviseur
        del self.monitor_supervisor # Supprimer le superviseur

        self.serial_monitor.running = False # Arrêter le moniteur
        self.serial_monitor.join() # Attendre la fin du moniteur
        self.serial_monitor.close() # Fermer le port série
        del self.serial_monitor # Supprimer le moniteur

        # Quitter l'application
        self.quit()
        self.destroy()

    def health_check(self, event):
        """
        Afficher un rapport de santé du moniteur en série.

        Args:
            event (tk.Event): L'événement déclencheur.
        """
        res = f"""Health check
        port: {self.serial_monitor.port}
        baudrate: {self.serial_monitor.baudrate}
        running: {self.serial_monitor.running}
        open: {self.serial_monitor.is_open}"""
        print(res)
        self.log(res)
