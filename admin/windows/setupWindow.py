import tkinter as tk
from tkinter import ttk
from time import sleep
from serial.tools import list_ports
from threading import Thread

class Worker(Thread):
    """
    Classe représentant un thread de travail qui vérifie périodiquement
    les valeurs des entrées de port et de débit en bauds et met à jour l'état du bouton de connexion.

    Attributes:
        parent (Setup): La fenêtre principale pour accéder aux éléments de l'interface graphique.
        running (bool): État du thread.
    """
    def __init__(self, parent=None):
        super().__init__()
        self.daemon = True
        self.parent = parent
        self.running = False

    def run(self):
        """
        Tâche du thread : vérifier les valeurs des entrées et mettre à jour le bouton de connexion.
        """
        if self.parent:
            while self.running:
                self.parent.check_baudrate_value()
                self.parent.check_port_value()
                self.parent.update_connect_button()
                sleep(0.03)
        print("Worker thread stopped")

    def start(self) -> None:
        """
        Démarrer le thread.
        """
        self.running = True
        return super().start()

class Setup(tk.Tk):
    """
    Classe représentant l'interface de configuration pour le moniteur série.

    Attributes:
        valid_port (bool): État de validation du port série.
        valid_baudrate (bool): État de validation du débit en bauds.
        worker (Worker): Thread de travail pour vérifier les entrées.
        ports (list): Liste des ports disponibles.
        selected_port (tk.StringVar): Port série sélectionné.
        baudrate (tk.StringVar): Débit en bauds sélectionné.
    """
    def __init__(self):
        super().__init__()
        self.title("Serial Monitor Setup")
        self.geometry("500x180")
        self.minsize(500, 180)

        # État des entrées pour vérifier si elles sont valides
        self.valid_port = True
        self.valid_baudrate = True

        # Thread de travail pour vérifier les valeurs des entrées
        self.worker = Worker(self)

        # Déclaration des variables pour stocker les données à retourner
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

        # Titre de la fenêtre
        self.title_label = tk.Label(self, text="Serial Monitor Setup", font=("Arial", 20))
        self.title_label.grid(row=0, column=0, columnspan=7)

        # Combobox pour sélectionner le port série
        self.port_label = tk.Label(self, text="Serial port 1")
        self.port_label.grid(row=1, column=1)

        self.port_combobox = ttk.Combobox(self, textvariable=self.selected_port, values=self.ports)
        self.port_combobox.grid(row=1, column=2, columnspan=3, sticky="ew", padx=5, pady=5)
        self.columnconfigure(2, weight=1)
        self.port_combobox.current(0)

        # Bouton pour rafraîchir la liste des ports
        self.refresh_button = tk.Button(self, text="Refresh", command=self.refresh_ports)
        self.refresh_button.grid(row=1, column=5, sticky="ew", padx=5, pady=5)

        # Entrée pour le débit en bauds
        self.baudrate_label = tk.Label(self, text="Baudrate")
        self.baudrate_label.grid(row=3, column=1, sticky="ew")

        self.baudrate_entry = tk.Entry(self, textvariable=self.baudrate)
        self.baudrate.set("19200")
        self.baudrate_entry.grid(row=3, column=2, columnspan=2, sticky="ew", padx=5, pady=5)

        # Bouton de connexion
        self.connect_button = tk.Button(self, text="Connect", command=self.exit)
        self.connect_button.config(state=tk.DISABLED)
        self.connect_button.grid(row=4, column=2, columnspan=3, sticky="ew", padx=5, pady=5)

        # Attacher l'événement de fermeture de la fenêtre à une fonction personnalisée
        self.protocol("WM_DELETE_WINDOW", self.quit)

        # Démarrer le thread de travail
        self.worker.start()

    def check_baudrate_value(self, event=None):
        """
        Vérifier si le débit en bauds entré est valide.
        """
        try:
            int(self.baudrate.get())
            self.valid_baudrate = True
        except ValueError:
            self.valid_baudrate = False
            print('Invalid baudrate')

    def check_port_value(self, event=None):
        """
        Vérifier si le port sélectionné est valide.
        """
        if self.selected_port.get() in self.ports:
            self.valid_port = True
        else:
            self.valid_port = False

    def update_connect_button(self):
        """
        Mettre à jour l'état du bouton de connexion en fonction de la validité des entrées.
        """
        if self.valid_port and self.valid_baudrate:
            self.connect_button.config(state=tk.NORMAL)
        else:
            self.connect_button.config(state=tk.DISABLED)

    def set_default_baudrate(self, event=None):
        """
        Définir le débit en bauds par défaut à 9600.
        """
        self.baudrate.set("9600")

    def get_available_ports(self):
        """
        Obtenir la liste des ports disponibles.

        Returns:
            list: Liste des ports disponibles.
        """
        return sorted(list_ports.comports())

    def refresh_ports(self):
        """
        Rafraîchir la liste des ports disponibles.
        """
        self.ports = [f"{port.name} - {port.description}" for port in self.get_available_ports()]
        self.port_combobox['values'] = self.ports

    def get_data(self):
        """
        Obtenir les données entrées par l'utilisateur après la fermeture de la fenêtre.

        Returns:
            dict: Dictionnaire contenant les données de port et de débit en bauds.
        """
        self.mainloop()
        data = {
            "port": self.selected_port.get().split(" - ")[0],
            "baudrate": int(self.baudrate.get())
        }
        if isinstance(data["baudrate"], int):
            return data
        else:
            return {"error": "Invalid data", "data": data}

    def exit(self):
        """
        Arrêter le thread de travail et fermer la fenêtre.
        """
        self.worker.running = False
        self.worker.join()
        super().quit()
        super().destroy()

if __name__ == "__main__":
    app = Setup()
    print(app.get_data())
