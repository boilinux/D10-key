import 'dart:convert';
import 'dart:developer';
import 'dart:io';

import 'package:camera/camera.dart';
import 'package:http/http.dart' as http;
import 'package:flutter/foundation.dart';
import 'package:shared_preferences/shared_preferences.dart';

String server = '82.180.137.10';

class Api {
  List<dynamic> nid = [];
  Api();

  //
  Future<void> getNodeId() async {
    try {
      var headers = {
        //HttpHeaders.authorizationHeader: token,
        HttpHeaders.contentTypeHeader: 'application/json',
        'Authorization': 'e0gXIbic8HBhpytccpuo6Xnrbic8HBhpytc2cxigF11LIZ8',
      };

      // inspect(id);

      final url = Uri.parse('http://$server/api/v1/getnodeid2');
      final response = await http.get(
        url,
        headers: headers,
      );
      final extractedData =
          json.decode(utf8.decode(response.bodyBytes)) as Map<String, dynamic>;

      inspect(extractedData);
      print(extractedData);
      nid = extractedData['nid'];

      // ignore: unnecessary_null_comparison
      if (extractedData == null) {
        return;
      }
      inspect(extractedData);
    } catch (error) {
      rethrow;
    }
  }

  Future<void> updateNodeImage(File image) async {
    try {
      final url = Uri.parse('http://$server/api/v1/updatenodewithimage');
      var request = http.MultipartRequest('POST', url)
        ..headers['Authorization'] =
            'e0gXIbic8HBhpytccpuo6Xnrbic8HBhpytc2cxigF11LIZ8'
        ..fields['what_image'] = 'back'
        ..fields['nid'] = json.encode(nid);
      if (image != '') {
        request.files
            .add(await http.MultipartFile.fromPath('image', image.path));
      }
      var streamResponse = await request.send();
      var response = await http.Response.fromStream(streamResponse);
      final extractedData =
          json.decode((response.body)) as Map<String, dynamic>;

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
