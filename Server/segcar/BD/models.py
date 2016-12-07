from django.db import models

class Rua(models.Model):
    nome = models.CharField(max_length=100)
    dia = models.IntegerField(default=0)
    hora = models.IntegerField(default=0)
    risco = models.FloatField(default=1)
    vel = models.IntegerField(default=40)
    def __str__(self):
        return self.nome + self.dia+"\\"+ " = " + str(self.risco)

class Carro(models.Model):
    modelo = models.CharField(max_length=100)
    proprietario = models.CharField(max_length=200)
    fabricacao = models.IntegerField(default = 0)
    pkt = models.IntegerField(default=0)
    q_rua = models.IntegerField(default = 0)
    v_rua = models.IntegerField(default = 0)
    def __str__(self):
        return self.modelo +"  "+self.proprietario

class Ano(models.Model):
    carro = models.ForeignKey(Carro, on_delete=models.CASCADE)
    ano = models.IntegerField(default = 0)
    def tarifa(self):
        tarifa = 0
        for mes in Mes.objects.filter(ano = self):
            tarifa = tarifa + mes.tarifa()
        return tarifa
    def __str__(self):
        return self.carro.proprietario + " " +str(self.ano)+" = "+ str(self.tarifa())

class Mes(models.Model):
    ano = models.ForeignKey(Ano, on_delete=models.CASCADE)
    mes = models.IntegerField(default = 0)
    def tarifa(self):
        tarifa = 0
        for dia in Dia.objects.filter(mes = self):
            tarifa = tarifa + dia.tarifa
        return tarifa
    def __str__(self):
        return str(self.mes) + " = " + str(self.tarifa())

class Dia(models.Model):
    mes = models.ForeignKey(Mes, on_delete=models.CASCADE)
    tarifa = models.FloatField(default = 0)
    dia = models.IntegerField(default = 0)
    def __str__(self):
        return str(self.dia) + " = " + str(self.tarifa)

class Msg(models.Model):
    id_carro = models.IntegerField(default=0)
    pkt = models.IntegerField(default=0)
    gps = models.CharField(max_length=40)
    vel = models.IntegerField(default=0)

    def insert(self):
        self.save()

    def __str__(self):
        return str(self.id_carro) + ": Estou aqui :D"

class Historico(models.Model):
    carro = models.ForeignKey(Carro, on_delete=models.CASCADE);
    timestamp =models.CharField(max_length=20)
    pkt = models.IntegerField(default=0)
    gps = models.CharField(max_length=40)
    vel = models.IntegerField(default=0)
    def __str__(self):
        return str(self.carro.modelo)+ "\t\tpacote=" + str(self.pkt) + "\t" + "hora = "+ str(self.timestamp)