from django.shortcuts import render, redirect, get_object_or_404
from .models import Document

def home(request):
    documents = Document.objects.all()
    return render(request, 'editor.html', {'documents': documents})

def create_document(request):
    if request.method == 'POST':
        title = request.POST['title']
        content = request.POST['content']
        Document.objects.create(title=title, content=content)
        return redirect('home')
    return render(request, 'editor.html')

def edit_document(request, doc_id):
    document = get_object_or_404(Document, id=doc_id)
    if request.method == 'POST':
        document.title = request.POST['title']
        document.content = request.POST['content']
        document.save()
        return redirect('home')
    return render(request, 'editor.html', {'document': document})

def delete_document(request, doc_id):
    document = get_object_or_404(Document, id=doc_id)
    document.delete()
    return redirect('home')
