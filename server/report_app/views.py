from rest_framework import viewsets
from rest_framework.decorators import action
from rest_framework.response import Response

from report_app.models import Point
from report_app.serializers import PointSerializer


class PointViewSet(viewsets.ModelViewSet):
    queryset = Point.objects.all()
    serializer_class = PointSerializer

    @action(detail=False, methods=['GET', 'DELETE'])
    def remove_all(self, *args, **kwargs):
        Point.objects.all().delete()
        return Response('done')


# class PointRangeViewSet(viewsets.ModelViewSet):
#     queryset = PointRange.objects.all()
#     serializer_class = PointRangeSerializer
