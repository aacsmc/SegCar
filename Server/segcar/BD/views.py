from django.shortcuts import render, get_object_or_404
from rest_framework.views import APIView
from rest_framework.response import Response
from rest_framework import status
from rest_framework.views import APIView
from .models import Carro
from .serializers import CarroSerializer
from rest_framework.response import Response
from django.http import Http404

class TodosCarros(APIView):

	def get(self, request):
		serializer = CarroSerializer(Carro.objects.all(), many = True)
		return Response(serializer.data)
	def	post(self):
		pass

class CondicaoCarro(APIView):

	def get(self, request, carro_id):
		serializer = CarroSerializer(Carro.objects.get(pk=carro_id), many = False)
		return Response(serializer.data)
	def	post(self):
		pass

# Create your views here.from django.shortcuts import render

# Create your views here.
