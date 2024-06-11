import tkinter as tk
from tkinter import scrolledtext
from threading import Thread
from time import sleep

from serial import Serial

class Controller(tk.Tk):
    """
    Classe représentant l'interface de contrôle pour le moniteur série.

    Attributes:
        serial (Serial): Objet série pour la communication.
        port (str): Port série sélectionné.
        baudrate (int): Débit en bauds sélectionné.
    """
    def __init__(self, port: str, baudrate: int):
        super().__init__()
        self.title("Pas Vraiment Guignol...")
        self.geometry("500x800")
        self.minsize(500, 300)

        self.port = port
        self.baudrate = baudrate

        self.serial = Serial(port, baudrate)
        if not self.serial.is_open:
            self.serial.open()
        else:
            print(f"Port {port} already open")
            self.serial.close()
            self.serial.open()

        self.create_widgets()

        # reassigner la fermerture de la fenêtre pour fermer le port série
        self.protocol("WM_DELETE_WINDOW", self.close)

        # variable pour stocker les données à envoyer
        self.axis = {"left_x": 0, "left_y": 0, "right_x": 0, "right_y": 0}

        # variable pour stocker l'état de la boucle d'envoi
        self.running = True

        # fil d'exécution pour envoyer les données
        self.thread = Thread(target=self.run)
        self.thread.start()

    def create_widgets(self) -> None:
        """
        Créer les widgets de l'interface.
        """
        self.title = tk.Label(self, text="Pas Vraiment Guignol...", font=("Arial", 20))
        self.title.pack()

        self.text = scrolledtext.ScrolledText(self, wrap=tk.WORD)
        self.text.pack(fill="both", expand=True, side=tk.TOP)

        self.left_frame = tk.Frame(self, height=100, width=100)
        self.left_frame.pack(side="left", fill="both", expand=True, padx=5, pady=5)

        self.right_frame = tk.Frame(self, height=100, width=100)
        self.right_frame.pack(side="right", fill="both", expand=True, padx=5, pady=5)
        
        # Configuration des lignes et colonnes pour les frames (permet de redimensionner les boutons)
        self.left_frame.grid_rowconfigure(0, weight=1)
        self.left_frame.grid_rowconfigure(1, weight=1)
        self.left_frame.grid_rowconfigure(2, weight=1)
        self.left_frame.grid_columnconfigure(0, weight=1)
        self.left_frame.grid_columnconfigure(1, weight=1)
        self.left_frame.grid_columnconfigure(2, weight=1)

        self.right_frame.grid_rowconfigure(0, weight=1)
        self.right_frame.grid_rowconfigure(1, weight=1)
        self.right_frame.grid_rowconfigure(2, weight=1)
        self.right_frame.grid_columnconfigure(0, weight=1)
        self.right_frame.grid_columnconfigure(1, weight=1)
        self.right_frame.grid_columnconfigure(2, weight=1)

        self.left_top_button = tk.Button(self.left_frame, font=("Arial", 20))
        self.left_top_button.config(bg='cyan')
        self.left_top_button.bind("<ButtonPress>", lambda e: self.set_axis("left_y", 1))
        self.left_top_button.bind("<ButtonRelease>", lambda e: self.set_axis("left_y", 0))
        self.left_top_button.grid(row=0, column=1, sticky="nsew")

        self.left_left_button = tk.Button(self.left_frame, font=("Arial", 20))
        self.left_left_button.config(bg='cyan')
        self.left_left_button.bind("<ButtonPress>", lambda e: self.set_axis("left_x", 2))
        self.left_left_button.bind("<ButtonRelease>", lambda e: self.set_axis("left_x", 0))
        self.left_left_button.grid(row=1, column=0, sticky="nsew")

        self.left_right_button = tk.Button(self.left_frame, font=("Arial", 20))
        self.left_right_button.config(bg='cyan')
        self.left_right_button.bind("<ButtonPress>", lambda e: self.set_axis("left_x", 1))
        self.left_right_button.bind("<ButtonRelease>", lambda e: self.set_axis("left_x", 0))
        self.left_right_button.grid(row=1, column=2, sticky="nsew")

        self.left_bottom_button = tk.Button(self.left_frame, font=("Arial", 20))
        self.left_bottom_button.config(bg='cyan')
        self.left_bottom_button.bind("<ButtonPress>", lambda e: self.set_axis("left_y", 2))
        self.left_bottom_button.bind("<ButtonRelease>", lambda e: self.set_axis("left_y", 0))
        self.left_bottom_button.grid(row=2, column=1, sticky="nsew")

        self.right_top_button = tk.Button(self.right_frame, font=("Arial", 20))
        self.right_top_button.config(bg='cyan')
        self.right_top_button.bind("<ButtonPress>", lambda e: self.set_axis("right_y", 1))
        self.right_top_button.bind("<ButtonRelease>", lambda e: self.set_axis("right_y", 0))
        self.right_top_button.grid(row=0, column=1, sticky="nsew")

        self.right_left_button = tk.Button(self.right_frame, font=("Arial", 20))
        self.right_left_button.config(bg='cyan')
        self.right_left_button.bind("<ButtonPress>", lambda e: self.set_axis("right_x", 2))
        self.right_left_button.bind("<ButtonRelease>", lambda e: self.set_axis("right_x", 0))
        self.right_left_button.grid(row=1, column=0, sticky="nsew")

        self.right_right_button = tk.Button(self.right_frame, font=("Arial", 20))
        self.right_right_button.config(bg='cyan')
        self.right_right_button.bind("<ButtonPress>", lambda e: self.set_axis("right_x", 1))
        self.right_right_button.bind("<ButtonRelease>", lambda e: self.set_axis("right_x", 0))
        self.right_right_button.grid(row=1, column=2, sticky="nsew")

        self.right_bottom_button = tk.Button(self.right_frame, font=("Arial", 20))
        self.right_bottom_button.config(bg='cyan')  
        self.right_bottom_button.bind("<ButtonPress>", lambda e: self.set_axis("right_y", 2))
        self.right_bottom_button.bind("<ButtonRelease>", lambda e: self.set_axis("right_y", 0))
        self.right_bottom_button.grid(row=2, column=1, sticky="nsew")

        

    def set_axis(self, axis: str, value: int) -> None:
        """
        Modifier la valeur d'un axe.

        Args:
            axis (str): Nom de l'axe à modifier.
            value (int): Valeur à ajouter à l'axe.
        """
        self.axis[axis] = value

    def send(self) -> None:
        """
        Envoyer le texte de l'entrée série.
        """
        payload = f"{self.axis['left_x']}{self.axis['left_y']}{self.axis['right_x']}{self.axis['right_y']}\n".encode()
        print(payload)
        self.serial.write(payload)

    def run(self) -> None:
        """
        Boucle d'envoi vers le port série.
        """
        while self.running:
            # si des messages sont disponibles, les lire
            if self.serial.in_waiting:
                try:
                    message = self.serial.readline().decode().strip()
                    self.text.insert(tk.END, f"{message}\n")
                    self.text.see(tk.END)
                except UnicodeDecodeError:
                    print("Error decoding message")
            # si une des valeurs est différente de 0, envoyer les données
            if any(self.axis.values()):
                self.send()
            # envoyer les messages  en attente
            try:
                self.serial.flush()
            except:
                pass
        


    def close(self) -> None:
        """
        Fermer le port série et détruire la fenêtre.
        """
        self.serial.close()
        self.running = False
        # avorter les opération sur le port série
        try:
            self.serial.cancel_read()
        except:
            pass
        self.thread.join()
        self.destroy()

if __name__ == "__main__":
    controller = Controller("COM3", 9600)
    controller.mainloop()