from django.urls import path
from . import views

urlpatterns = [
    path('', views.home, name='home'),
    path('create/', views.create_document, name='create_document'),
    path('edit/<int:doc_id>/', views.edit_document, name='edit_document'),
    path('delete/<int:doc_id>/', views.delete_document, name='delete_document'),
]
