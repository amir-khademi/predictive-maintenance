from rest_framework import serializers

from report_app.models import Point


class PointSerializer(serializers.ModelSerializer):
    # status = serializers.SerializerMethodField()

    class Meta:
        model = Point
        fields = '__all__'

#     def get_status(self, obj):
#         if obj.value > 3000:
#             return True
#
#         else:
#             return False
#
#
# class PointRangeSerializer(serializers.ModelSerializer):
#     status = serializers.SerializerMethodField()
#
#     class Meta:
#         model = PointRange
#         fields = '__all__'
#
#     def get_status(self, obj):
#         for point in Point.objects.filter(id__in=range(obj.start_point.id, obj.end_point.id)):
#             if point.value > 3000:
#                 return True
#         else:
#             return False


# class DetectionSerializer(serializers.Serializer):
#     status = serializers.SerializerMethodField()
#
#     def get_status(self):
#