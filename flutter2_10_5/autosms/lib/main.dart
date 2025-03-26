import 'dart:async';
import 'dart:io';

import 'package:flutter/material.dart';
import 'package:telephony/telephony.dart';
import './api/request.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  final Telephony telephony = Telephony.instance;
  Api api = Api();
  bool _loader = false;

  Timer? _timer;

  @override
  void initState() {
    super.initState();
    //_startGetSmsNotification();
  }

  void _startGetSmsNotification() {
    _timer = Timer.periodic(const Duration(seconds: 2), (timer) {
      getSmsNotification();
    });
  }

  void getSmsNotification() {
    setState(() {
      api.getSmsNotification();
      //api.sendSms(telephony);
    });
  }

  void sendSms(String phoneNumber, String message) async {
    bool? permissionGranted = await telephony.requestSmsPermissions;
    if (permissionGranted ?? false) {
      setState(() {
        api.sendSms(telephony);
      });
    } else {
      print("SMS Permission Denied!");
    }
  }

  @override
  void didChangeDependencies() {
    super.didChangeDependencies();
    // Perform operations that depend on inherited widgets
  }

  @override
  void dispose() {
    _timer?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(title: const Text("SMS Sender")),
        body: RefreshIndicator(
          onRefresh: () async {
            setState(() {
              api.getSmsNotification();
              _loader = false;
            });
          },
          child: Center(
            child: ListView(
              children: [
                const Center(
                  child: Text(
                    'Messages',
                    style: TextStyle(fontSize: 28, color: Colors.green),
                  ),
                ),
                for (var record in api.nodes)
                  Card(
                    child: Padding(
                      padding: const EdgeInsets.all(8),
                      child: Text(
                        record['title'],
                        style: const TextStyle(fontSize: 24),
                      ),
                    ),
                  ),
                const Center(
                  child: Text(
                    'Phone Numbers',
                    style: TextStyle(fontSize: 28, color: Colors.green),
                  ),
                ),
                for (var record in api.phoneNumbers)
                  Card(
                    child: Padding(
                      padding: const EdgeInsets.all(8),
                      child: Text(
                        record,
                        style: const TextStyle(fontSize: 24),
                      ),
                    ),
                  ),
                ElevatedButton(
                    onPressed: () async {
                      setState(() {
                        _loader = true;
                      });
                      api.sendSms(telephony);
                      setState(() {
                        _loader = false;
                      });
                    },
                    child: _loader
                        ? const CircularProgressIndicator(
                            color: Colors.green,
                          )
                        : const Text("Send SMS"))
              ],
            ),
          ),
        ),
      ),
    );
  }
}
