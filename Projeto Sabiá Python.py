import xml.etree.ElementTree as ET
from bluepy.btle import Peripheral
addr = '3C:71:BF:6B:D9:AA'
tree = ET.parse('fadarabo.musicxml')
root = tree.getroot()
supp = ['C', 'D', 'E', 'F', 'G', 'A', 'B']
notas = []
bType = 0
refTime = 0
nTipos = ['1024th', '512th', '256th', '128th', '64th', '32nd', '16th',
          'eighth', 'quarter', 'half', 'whole', 'breve', 'long', 'maxima']
nMultiplos = [1/256, 1/128, 1/64,
              1/32, 1/16, 1/8,
              1/4, 1/2, 1, 2,
              4, 8, 16, 32]
nTempos = [1/256, 1/128, 1/64,
           1/32, 1/16, 1/8,
           1/4, 1/2, 1, 2, 4,
           8, 16, 32]
refViolino = ['d4', 'C#4', 'C4', 'B3', 'A#3', 'A3', 'G#3', 'G3',
              'a4', 'G#4', 'G4', 'F#4', 'F4', 'E4', 'D#4', 'D4',
              'e5', 'D#5', 'D5', 'C#5', 'C5', 'B4', 'A#4', 'A4',
              'B5', 'A#5', 'A5', 'G#5', 'G5', 'F#5', 'F5', 'E5']
bpm = int(input('Escolha as BPM: '))

for measure in root[-1]:
    for note in measure:
        nota = []
        if note.tag == 'barline' or note.tag == 'print':
            continue
        if note.tag == 'attributes':
            bType = 4*(1/float(note[2][1].text))
            refTime = (bType*60)/bpm
            continue
        a = 0
        noteItems = [(x.tag, x.text) for x in note]
        pitchItems = [(x.tag, x.text) for x in note[0]]

        for i in noteItems:
            if i[0] == 'accidental':
                nota.append(i[1])
                continue
            if i[0] == 'type':
                nota.append(i[1])
                continue
            if i[0] == 'rest':
                nota.append('rest')
                continue

        for i in pitchItems:
            if i[0] == 'octave':
                nota.append(i[1])
                continue
            if i[0] == 'step':
                nota.append(i[1])
                continue
        if len(nota) < 4:
            nota.insert(1, 'natural')
        if 'rest' in nota:
            nota.append('rest')
        notas.append(nota)

for i in notas:
    if i[1] == 'flat':
        if i[2] == 'C' or i[2] == 'F':
            i[2] = supp[supp.index(i[2])-1]
            i[1] = 'natural'
            continue
        i[1] = 'sharp',
        i[2] = supp[supp.index(i[2])-1]
    if i[1] == 'sharp':
        if i[2] == 'E' or i[2] == 'B':
            i[2] = supp[supp.index(i[2]) + 1]
            i[1] = 'natural'

print(notas)
notasF = []
for i in notas:
    nTexto = ''
    if len(i) == 2:
        print(i)
    nTexto += i[2]
    if i[1] == 'sharp':
        nTexto += '#'
    nTexto += i[3]
    notaF = [refViolino.index(nTexto), int(1000*(refTime*nTempos[nTipos.index(i[0])]))]
    notasF.append(notaF)
print(notasF)

ble_data = []
for i in notasF:
    suppString = ''
    if int(i[1] / 1000) == 0:
        suppString = '0'+str(i[1])
    else:
        suppString = str(i[1])
    if len(str(i[0])) == 1:
        suppString = '0'+str(i[0])+suppString
    else:
        suppString = str(i[0])+suppString
    ble_data.append(str.encode(suppString))
    # if int(i[1]/1000) == 0:
    #     ble_data.append(str.encode(str(i[0])+'0'+str(i[1])))
    # elif len(str(i[0])) == 1:
    #     ble_data.append(str.encode('0'+str(i[0])+'0'+str(i[1])))
    # else:
    #     ble_data.append(str.encode(str(i[0])+str(i[1])))
print(ble_data)
input()
p = Peripheral(addr, 'public')
s = p.getServiceByUUID("20ff121c-94e1-11e9-bc42-526af7764f64")
print("conectado")
carachteristics = s.getCharacteristics()[0]
for j in ble_data:
    carac = int(carachteristics.read())
    print(carac)
    # p.waitForNotifications()
    while carac != 33:
        if carac == 32:
            break
        pass
    carachteristics.write(j)
    print(j)
p.disconnect()
