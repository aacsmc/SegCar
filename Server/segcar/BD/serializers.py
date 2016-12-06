from rest_framework import serializers
from .models import Carro, Msg


class CarroSerializer(serializers.ModelSerializer):
    class Meta:
        model = Carro
        fields = ('q_rua', 'v_rua')
    # fields = '__all__'


class MessageSerializer(serializers.ModelSerializer):
    class Meta:
        model = Msg
        fields = '__all__'
