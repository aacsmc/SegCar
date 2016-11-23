from rest_framework import serializers
from .models import Carro

class CarroSerializer(serializers.ModelSerializer):

	class Meta:
		model = Carro
		#fields = ('q_rua', 'v_rua')
		fields = '__all__'