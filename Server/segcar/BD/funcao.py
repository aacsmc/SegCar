from geopy.geocoders import Nominatim
import unicodedata
import datetime
from django.utils import timezone
from .models import Carro, Rua, Ano, Mes, Dia, Historico
#import pytz

geolocator = Nominatim()
#time.activate("Etc/GMT-3")
#tz = datetime.timezone('Etc/GMT-3')
def get_nome(end):
    i = 0
    f = 0
    f2 = 0
    cont = 0
    for c in end:
        if c == ',':
            cont += 1
        if cont == 0:
            i += 1
        if cont <= 1:
            f += 1
        if cont<=2:
            f2+=1

    if ("AVENIDA" == end.upper()[0:7] or "RUA" == end.upper()[0:4]):
        return filter(end[0:i])
    if  ("AVENIDA" == end.upper()[i+2:i+9] or "RUA" == end.upper()[i+2:i+6]):
        return filter(end[i + 2:f].upper())
    return filter(end[f+2:f2]).upper()

def filter(s):
    s = unicodedata.normalize('NFKD', s).encode('ascii', 'ignore')
    s = str(s)[2:len(str(s)) - 1]
    if ("AVENIDA" == s.upper()[0:7]):
        s = "AV" + s.upper()[7:]
    s = s.upper()
    print(s)
    return s


def calcula_ic(rua, vel):
    conducao = (rua.risco + 1) * vel
    #print(rua.nome + " " + str(rua.risco))
    if vel > rua.vel:
        conducao *= 2
    return conducao


def get_rua(gps):
    now = timezone.localtime(timezone.now())
    dia = now.weekday()
    hora = int(now.hour) / 3
    location = geolocator.reverse(gps)
    nome_rua = get_nome(location.address)

    try:
        rua = Rua.objects.get(hora=hora, dia=dia, nome=nome_rua)
    except Rua.DoesNotExist:
        rua = Rua.objects.get(nome="NAO ENCONTRADO")

    return rua


def update_carro(msg):
    now = timezone.localtime(timezone.now())

    try:
        car = Carro.objects.get(pk=msg.id_carro)
    except Carro.DoesNotExist:
        return
    if not Ano.objects.filter(carro=car, ano=int(now.year)).exists():
        a = Ano(carro=car, ano=int(now.year))
        a.save()

    year = Ano.objects.get(carro=car, ano=int(now.year))

    if not Mes.objects.filter(ano=year, mes=int(now.month)).exists():
        a = Mes(ano=year, mes=int(now.month))
        a.save()
    month = Mes.objects.get(ano=year, mes=int(now.month))

    if not Dia.objects.filter(mes=month, dia=int(now.day)).exists():
        a = Dia(mes=month, dia=int(now.day))
        a.save()
    day = Dia.objects.get(mes=month, dia=int(now.day))

    rua = get_rua(msg.gps)

    tar = day.tarifa + calcula_ic(rua, msg.vel)
    day.tarifa = tar
    day.save()
    update_hist(car, msg)
    update_feedback(car, rua, msg.vel, msg.pkt)


def update_hist(car, msg):
    try:
        hist = Historico.objects.get(pkt=(msg.pkt % 720), carro=car)
    except Historico.DoesNotExist:
        hist = Historico(carro=car, pkt=msg.pkt)

    hist.vel = msg.vel
    hist.gps = msg.gps

    if msg.pkt > car.pkt:
        hist.timestamp = timezone.localtime(timezone.now()).strftime("%c")
    else:
        hist.timestamp = "atrasado"
    car.pkt=max(car.pkt,msg.pkt)
    car.save()
    hist.save()


def update_feedback(car, rua, vel, pkt):
    if (pkt < car.pkt):
        return
    if (rua.risco < 1):
        car.q_rua = 1
    elif (rua.risco < 5):
        car.q_rua = 2
    else:
        car.q_rua = 3

    if (rua.vel < vel):
        car.v_rua = 1
    else:
        car.v_rua = 0

    car.save()
