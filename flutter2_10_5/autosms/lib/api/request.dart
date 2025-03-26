import 'dart:convert';
import 'dart:developer';
import 'dart:io';

import 'package:http/http.dart' as http;
import 'package:telephony/telephony.dart';

String server = '82.180.137.10';

class Api {
  List<dynamic> nodes = [];
  List<dynamic> phoneNumbers = [];
  Api();

  //
  Future<void> getSmsNotification() async {
    try {
      var headers = {
        HttpHeaders.contentTypeHeader: 'application/json',
        'Authorization': 'e0gXIbic8HBhpytccpuo6Xnrbic8HBhpytc2cxigF11LIZ8',
      };

      // inspect(id);

      final url = Uri.parse('http://$server/api/v1/getsmsnotification');
      final response = await http.get(
        url,
        headers: headers,
      );
      final extractedData =
          json.decode(utf8.decode(response.bodyBytes)) as Map<String, dynamic>;

      //print(extractedData);
      nodes = extractedData['nodes'];
      phoneNumbers = extractedData['phone_numbers'];
      print(phoneNumbers);
      print(nodes);

      // ignore: unnecessary_null_comparison
      if (extractedData == null) {
        return;
      }
      inspect(extractedData);
    } catch (error) {
      rethrow;
    }
  }

  Future<void> sendSms(Telephony sms) async {
    if (nodes.isEmpty || phoneNumbers.isEmpty) {
      return;
    }

    // iterate phone numbers
    for (var row in phoneNumbers) {
      String phoneNumberStr = row;
      // iterate nodes
      for (var row in nodes) {
        String title = row['title'];
        sms.sendSms(
          to: phoneNumberStr,
          message: title,
          statusListener: (SendStatus status) async {
            if (status == SendStatus.SENT || status == SendStatus.DELIVERED) {
              //await postSmsNotification(row['nid']);
              print("SMS Sent Successfully!");
            } else {
              print("SMS Failed to Send!");
            }
          },
        );
        await postSmsNotification(row['nid']);
        print("Title is ${row['title']}");
      }
    }
  }

  Future<void> postSmsNotification(dynamic nid) async {
    try {
      var headers = {
        HttpHeaders.contentTypeHeader: 'application/json',
        'Authorization': 'e0gXIbic8HBhpytccpuo6Xnrbic8HBhpytc2cxigF11LIZ8',
      };

      // inspect(id);

      final url = Uri.parse('http://$server/api/v1/postsmsnotification');
      final response = await http.post(
        url,
        headers: headers,
        body: jsonEncode({
          'nid': [nid]
        }),
      );
      final extractedData =
          json.decode(utf8.decode(response.bodyBytes)) as Map<String, dynamic>;

      print(extractedData);

      // ignore: unnecessary_null_comparison
      if (extractedData == null) {
        return;
      }
      inspect(extractedData);
    } catch (error) {
      rethrow;
    }
  }
}
