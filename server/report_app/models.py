from django.db import models


class Point(models.Model):
    value = models.IntegerField()
    # datetime = models.DateTimeField()
    # datetime = models.DateTimeField(auto_now_add=True)

# class Pattern(models.Model):
#     value = models.TextField()
# file = models.FileField()


# class PointRange(models.Model):
#     start_point = models.ForeignKey(Point, on_delete=models.CASCADE, related_name='+')
#     end_point = models.ForeignKey(Point, on_delete=models.CASCADE, related_name='+')
