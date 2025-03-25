import 'dart:async';
import 'dart:convert';
import 'dart:ffi';
import 'dart:io';
import 'package:camera/camera.dart';
import 'package:flutter/material.dart';
import 'package:path_provider/path_provider.dart';

import './api/request.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  final cameras = await availableCameras();
  runApp(MyApp(cameras: cameras));
}

class MyApp extends StatelessWidget {
  final List<CameraDescription> cameras;

  const MyApp({Key? key, required this.cameras}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      home: CameraScreen(cameras: cameras),
    );
  }
}

class CameraScreen extends StatefulWidget {
  final List<CameraDescription> cameras;

  CameraScreen({Key? key, required this.cameras}) : super(key: key);

  @override
  _CameraScreenState createState() => _CameraScreenState();
}

class _CameraScreenState extends State<CameraScreen> {
  CameraController? _controller;
  Timer? _timer;
  bool _isCameraInitialized = false;

  @override
  void initState() {
    super.initState();
    _initializeCamera();
  }

  /// Initialize the camera
  Future<void> _initializeCamera() async {
    if (widget.cameras.isEmpty) {
      print("No cameras found");
      return;
    }
    // Select the front camera
    final frontCamera = widget.cameras!.firstWhere(
      (camera) => camera.lensDirection == CameraLensDirection.front,
    );

    _controller = CameraController(frontCamera, ResolutionPreset.medium);
    await _controller!.initialize();

    if (!mounted) return;

    setState(() {
      _isCameraInitialized = true;
    });

    //_startAutoCapture();
  }

  /// Start capturing images every minute
  void _startAutoCapture() {
    _timer = Timer.periodic(const Duration(seconds: 2), (timer) {
      _captureImage();
    });
  }

  /// Capture an image and save it
  Future<void> _captureImage() async {
    if (!_controller!.value.isInitialized) return;

    try {
      Api api = Api();
      await api.getNodeId();
      final XFile image = await _controller!.takePicture();
      // Read image file as bytes
      File imageFile = File(image.path);
      api.updateNodeImage(imageFile);
      //List<int> imageBytes = await imageFile.readAsBytes();

      //// Convert to Base64
      //String base64Image = base64Encode(imageBytes);

      //print("Base64 Image: $base64Image"); // Use this Base64 string as needed
      //final String filePath = await _saveImage(image);
      //print("Image saved: $filePath");
    } catch (e) {
      print("Error capturing image: $e");
    }
  }

  /// Save the image to the app's storage directory
  //Future<String> _saveImage(XFile image) async {
  //  final Directory directory = await getApplicationDocumentsDirectory();
  //  final String filePath =
  //      '${directory.path}/${DateTime.now().millisecondsSinceEpoch}.jpg';

  //  await File(image.path).copy(filePath);
  //  return filePath;
  //}

  @override
  void dispose() {
    _controller?.dispose();
    _timer?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text("Auto Capture Camera")),
      body: _isCameraInitialized
          ? ListView(
              children: [
                CameraPreview(_controller!),
                TextButton(
                    onPressed: _captureImage, child: const Text('Capture'))
              ],
            )
          : const Center(child: CircularProgressIndicator()),
    );
  }
}
