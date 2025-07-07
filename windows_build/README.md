# 🛡️ Anleitung für den Client der Gruppe **AD**

## 🔧 Ausführbare Datei

Auf die zu ausführende Datei für den Client heißt:

`client_launcher.exe`

Einfach Doppelkick darauf zum Starten.\
Da wir einen Subprozess starten, werden nach gewissen Rechten gefragt.
x## ⚙️ Konfigurationsdatei

Das Executable liest die (DNS-Adresse oder IP-Adresse) und den zu verwendenden Port aus der Konfigurationsdatei:

/clientInfo/config.txt

### 📄 Format der `config.txt`

Die Datei muss wie folgt aufgebaut sein (jeweils eine Angabe pro Zeile):

 ```<Adresse> <PORT> ```

Beispiel für lokalen Server:
```
localhost
5555
```
