# EC SPE

## Setup Serial
- Baud Rate: 9600
- Data bits: 8
- Parity: None
- Stop bits: 1
- Flow control: None
- Unit: KG

## Deployment with QT
1. Copy build serial.exe and resources
- bell_01.wav
- Noto Sans Folder
- c39hrp24dhtt.ttf
- mm_packing_weight.png
- sign0038.wav
  
2. Open Powershell/terminal, go to QT Directory e.g C:\Qt\6.6.3\mingw_64\bin
```
cd C:\Qt\6.6.3\mingw_64\bin
```

3. Run qtenv2.bat
```bash
.\qtenv2.bat
```

4. Run windeployqt.exe
```bash
.\windeployqt.exe "C:\Your\Path\serial.exe"
```

## For Linux
don't forget to add permission
```bash
sudo usermod -a -G dialout $USER
```