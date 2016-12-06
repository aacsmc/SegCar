from django.contrib import admin

from .models import Rua, Carro, Ano, Msg, Historico

admin.site.register(Rua)
admin.site.register(Carro)
admin.site.register(Ano)
admin.site.register(Msg)
admin.site.register(Historico)

# Register your models here.
