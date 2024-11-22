from django.db import models

# Create your models here.

class Document(models.Model):
    title = models.CharField(max_length=200)
    content = models.TextField(blank=True)
    last_modified = models.DateTimeField(auto_now=True)
    connected_users = models.JSONField(default=dict)
    version = models.IntegerField(default=0)

    def __str__(self):
        return self.title

class CursorPosition(models.Model):
    document = models.ForeignKey(Document, on_delete=models.CASCADE)
    user_id = models.CharField(max_length=100)
    position = models.IntegerField(default=0)
    last_update = models.DateTimeField(auto_now=True)
