from django.shortcuts import render, get_object_or_404
from rest_framework.views import APIView
from rest_framework.response import Response
from rest_framework import status
from rest_framework.views import APIView
from .models import Carro
from .serializers import CarroSerializer, MessageSerializer
from rest_framework.response import Response
from django.http import Http404, HttpResponse, JsonResponse
from django.utils.six import BytesIO
from rest_framework.renderers import JSONRenderer
from rest_framework.parsers import JSONParser


class TodosCarros(APIView):
    def get(self, request: object) -> object:
        serializer = CarroSerializer(Carro.objects.all(), many=True)
        return Response(serializer.data)

    def post(self):
        pass


class CondicaoCarro(APIView):
    def get(self, request, carro_id):
        try:
            carro = Carro.objects.get(pk=carro_id)
        except Carro.DoesNotExist:
            raise Http404("Carro does not exist!")
        serializer = CarroSerializer(carro, many=False)
        return Response(serializer.data)

    def post(self):
        pass


class UpdateUser(APIView):
    def post(self, request):
        if request.method == 'POST':
            data = JSONParser().parse(request)
            serializer = MessageSerializer(data=data)
            if serializer.is_valid(raise_exception=True):
                msegres = serializer.save()
                msegres.insert()
                return JsonResponse(serializer.data, status=201)
            return JsonResponse(serializer.errors, status=400)
