from windows.setupWindow import Setup
from windows.controller import Controller 

if __name__ == "__main__":
    app = Setup() # Instancier la fenêtre de configuration
    conf = app.get_data() # Récupérer les données de configuration (port, baudrate)
    if "error" in conf: # Si une erreur est survenue lors de la récupération des données, afficher le message d'erreur et quitter
        print(conf)
    else: # Sinon, instancier la fenêtre de moniteur série et la lancer
        monitor = Controller(conf["port"], conf["baudrate"])
        monitor.mainloop()