#!/usr/bin/python3
import sys
import firebase_admin
from firebase_admin import credentials
from firebase_admin import messaging


class Fcm:
    def __init__(self):
        cred = credentials.Certificate("/home/pi/dd.json")
        firebase_admin.initialize_app(cred)
        self.title = "Title"
        self.body = "Body"
        self.channel = "channel_info"
        self.topic = "INFO"

    def send(self):
        message = messaging.Message(
            android=messaging.AndroidConfig(
                priority='high',
                notification=messaging.AndroidNotification(
                    title=self.title,
                    body=self.body,
                    channel_id=self.channel,
                ),
            ),
            topic=self.topic,
        )
        return messaging.send(message)


if __name__ == '__main__':
    fcm = Fcm()
    fcm.channel = sys.argv[1]
    fcm.topic = sys.argv[2]
    fcm.title = sys.argv[3]
    fcm.body = sys.argv[4]
    response = fcm.send()
    print(response)

