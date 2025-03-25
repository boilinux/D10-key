import 'package:flutter/material.dart';
import 'package:telephony/telephony.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  final Telephony telephony = Telephony.instance;

  void sendSms() async {
    bool? permissionGranted = await telephony.requestSmsPermissions;
    if (permissionGranted ?? false) {
      telephony.sendSms(
        to: "+639760414205",
        message: "Hello World!",
        statusListener: (SendStatus status) {
          if (status == SendStatus.SENT) {
            print("SMS Sent Successfully!");
          } else if (status == SendStatus.DELIVERED) {
            print("SMS Delivered!");
          } else {
            print("SMS Failed to Send!");
          }
        },
      );
    } else {
      print("SMS Permission Denied!");
    }
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(title: Text("SMS Sender")),
        body: Center(
          child: ElevatedButton(
            onPressed: sendSms,
            child: Text("Send SMS"),
          ),
        ),
      ),
    );
  }
}
