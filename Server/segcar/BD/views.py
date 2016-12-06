from django.shortcuts import render, get_object_or_404
from rest_framework.views import APIView
from rest_framework.response import Response
from rest_framework import status
from rest_framework.views import APIView
from .models import Carro, Msg
from .serializers import CarroSerializer, MessageSerializer
from rest_framework.response import Response
from django.http import Http404, HttpResponse, JsonResponse
from django.utils.six import BytesIO
from rest_framework.renderers import JSONRenderer
from rest_framework.parsers import JSONParser
from BD.funcao import update_carro

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
            serializer = MessageSerializer(data=data,many=True)
            if serializer.is_valid(raise_exception=True):
                msg = serializer.save()
                x=msg[0]
                for i in msg:
                    update_carro(i)
                pk = msg[0].id_carro

                Msg.objects.all().delete()

                #msegres.insert()
                try:
                    carro = Carro.objects.get(pk=pk)
                    ret = CarroSerializer(carro, many=False)
                    if serializer.is_valid(raise_exception=True):
                        return JsonResponse(ret.data, status=201)
                except Carro.DoesNotExist:
                    pass
            return JsonResponse(serializer.errors, status=400)
