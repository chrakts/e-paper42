counter = 0
picRaw = list()
picComp = list()

directory = "/home/chrak/repositories/ePaperTerminal/pictures/"
name = "butterfly"
fnRaw = name+'.raw'
fnCmp = name+'.cmp'
varName = "image"+name.capitalize()

with open(directory+fnRaw) as f:
    lines = f.read().splitlines()
for l in lines:
  for i in l.split(','):
    if i!='':
      picRaw.append(int(i,16))
      counter+=1
db = [0] * 256
oldByte = -1
sameCounter = 0
for p in picRaw:
  if p==0 or p==255 or oldByte==0 or oldByte==255:
    if p==oldByte:
      sameCounter+=1
    else:
#      if oldByte!=-1:
      if sameCounter>0:
        if sameCounter>255:
          while(sameCounter>255):
            picComp.append(oldByte)
            picComp.append(255)
            sameCounter-=256
        picComp.append(oldByte)
        picComp.append(sameCounter)
        db[oldByte] += sameCounter
        sameCounter = 0
        if p!=0 and p!=255:
          picComp.append(p)
    oldByte = p
  else:
    picComp.append(p)
    
# falls am Ende mehrere 0xff 0der 0x00 stehen:
if sameCounter>0:
  if sameCounter>255:
    while(sameCounter>255):
      picComp.append(oldByte)
      picComp.append(255)
      sameCounter-=256
  picComp.append(oldByte)
  picComp.append(sameCounter)

# Kontrolle
contrCounter = 0
spc = False
for i in picComp:
  if spc == True:
    contrCounter+=i+1
    spc = False
  else:
    if(i==0 or i==255):
      spc = True
    else:
      contrCounter += 1
print (contrCounter)

with open(directory+fnCmp,'w') as f:
  f.write('const unsigned char '+varName +'[] PROGMEM = { \n')
  counter = 0
  for p in picComp:
    f.write('0x%02x,'%(p))
    counter+=1
    if counter%16 == 0:
      f.write('\n')
  f.write('};\n')
  
