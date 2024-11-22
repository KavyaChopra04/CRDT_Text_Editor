import json
import logging
from asgiref.sync import sync_to_async
from channels.generic.websocket import AsyncWebsocketConsumer
from .models import Document, CursorPosition

logger = logging.getLogger(__name__)

class CollaborationConsumer(AsyncWebsocketConsumer):
    async def connect(self):
        self.doc_id = self.scope['url_route']['kwargs']['doc_id']
        self.room_group_name = f"document_{self.doc_id}"
        self.user_id = f"user_{self.channel_name}"

        document = await sync_to_async(Document.objects.get)(id=self.doc_id)
        connected_users = document.connected_users
        connected_users[self.user_id] = {"cursor": 0}
        await sync_to_async(document.save)()

        logger.info(f"User {self.user_id} connected to document {self.doc_id}")

        await self.channel_layer.group_add(
            self.room_group_name,
            self.channel_name
        )
        await self.accept()

        # Send initial document state
        await self.send_initial_state()

    async def disconnect(self, close_code):
        document = await sync_to_async(Document.objects.get)(id=self.doc_id)
        connected_users = document.connected_users
        if self.user_id in connected_users:
            del connected_users[self.user_id]
        await sync_to_async(document.save)()

        await self.channel_layer.group_discard(
            self.room_group_name,
            self.channel_name
        )

    async def receive(self, text_data):
        data = json.loads(text_data)
        message_type = data.get('type', 'text_update')

        if message_type == 'cursor_update':
            await self.handle_cursor_update(data)
        else:
            await self.handle_text_update(data)

    async def handle_cursor_update(self, data):
        await self.channel_layer.group_send(
            self.room_group_name,
            {
                'type': 'broadcast_cursor',
                'user_id': self.user_id,
                'position': data['position']
            }
        )

    async def handle_text_update(self, data):
        document = await sync_to_async(Document.objects.get)(id=self.doc_id)
        content = document.content
        version = document.version if hasattr(document, 'version') else 0

        if data['action'] == 'add':
            new_content = content[:data['index']] + data['char'] + content[data['index']:]
        else:
            new_content = content[:data['index']] + content[data['index'] + 1:]

        document.content = new_content
        document.version = version + 1
        await sync_to_async(document.save)()

        await self.channel_layer.group_send(
            self.room_group_name,
            {
                'type': 'broadcast_update',
                'action': data['action'],
                'char': data.get('char'),
                'index': data['index'],
                'user_id': self.user_id
            }
        )

    async def broadcast_cursor(self, event):
        await self.send(text_data=json.dumps({
            'type': 'cursor_update',
            'user_id': event['user_id'],
            'position': event['position']
        }))

    async def broadcast_update(self, event):
        await self.send(text_data=json.dumps({
            'type': 'text_update',
            'action': event['action'],
            'char': event.get('char'),
            'index': event['index'],
            'user_id': event['user_id']
        }))

    async def send_initial_state(self):
        document = await sync_to_async(Document.objects.get)(id=self.doc_id)
        await self.send(text_data=json.dumps({
            'type': 'initial_state',
            'content': document.content,
            'connected_users': document.connected_users
        }))
        
        logger.info(f"Sent initial state to user {self.user_id}")
