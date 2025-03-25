<?php

declare(strict_types=1);

namespace Drupal\custom_module\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\HttpFoundation\JsonResponse;
use Symfony\Component\HttpFoundation\Request;
use Drupal\node\Entity\Node;
use Drupal\user\Entity\User;
use Drupal\file\Entity\File;
use Drupal\media\Entity\Media;
use Drupal\Core\File\FileSystemInterface;


/**
 * Returns responses for custom_module routes.
 */
final class CustomModuleController extends ControllerBase
{
  private function is_authorize(Request $request)
  {
    $requestToken = $request->headers->get('Authorization');
    $adminUser = User::load(1);
    $restToken = $adminUser->field_rest_token->value;

    if ($requestToken != $restToken) {
      return new JsonResponse(['error' => 'Access Denied'], 400);
    }
  }
  public function getNodeId(Request $request)
  {
    $this->is_authorize($request);

    $node = [];

    $query = \Drupal::database()->query(
      "SELECT nfd.nid FROM node_field_data AS nfd
      LEFT JOIN node__field_camera_photo AS nfcp ON nfcp.entity_id = nfd.nid
      WHERE nfcp.field_camera_photo_target_id IS NULL AND nfd.type = 'data_logs'"
    )->fetchAll();

    foreach ($query as $record) {
      $node[] = $record->nid;
    }

    return new JsonResponse(['message' => 'Node without image', 'nid' => $node], 200);
  }
  public function updateNodeImage(Request $request)
  {
    $this->is_authorize($request);

    $data['nid'] = json_decode($request->request->get('nid'), true);
    $data['image'] = $request->files->get('image');

    if (!isset($data['image']) || !isset($data['nid'])) {
      return new JsonResponse(['error' => 'Missing required data', 'data' => $data], 400);
    }

    try {

      if (is_array($data['nid'])) {
        foreach ($data['nid'] as $nid) {
          $node = Node::load($nid);

          // Handle file upload
          $files = $request->files->get('image'); // 'file' should match the key in Flutter request
          if (!$files) {
            return new JsonResponse(['error' => 'No file uploaded'], 400);
          }

          // Move file to Drupal private/public files
          $directory = 'public://uploads/';
          \Drupal::service('file_system')->prepareDirectory($directory, \Drupal\Core\File\FileSystemInterface::CREATE_DIRECTORY);

          $file_name = $files->getClientOriginalName();
          $file_destination = $directory . $file_name;
          $file_uri = \Drupal::service('file_system')->saveData(file_get_contents($files->getPathname()), $file_destination, FileSystemInterface::EXISTS_RENAME);

          \Drupal::logger('my_module')->error('File upload failed for: @file_name', ['@file_name' => $file_uri]);

          if ($file_uri) {
            // Convert to a Managed File Entity
            $file_entity = File::create([
              'uri' => $file_uri,
              'uid' => $node->uid, // Change this to the correct user ID if needed
              'status' => 1,
            ]);
            $file_entity->save();
          }

          $node->field_camera_photo->setValue([
            'target_id' => $file_entity->id(),
            'alt' => 'Photo camera shot',
            'title' => 'Photo camera shot',
          ]);

          $node->save();
        }
      }

      return new JsonResponse(['message' => 'Update node successfully', 'nid' => $data['nid']], 200);
    } catch (\Exception $e) {
      \Drupal::logger('my_module')->error('Error updating node: @error', ['@error' => $e->getMessage()]);
      return new JsonResponse(['error' => 'Error updating node: ' . $e->getMessage()], 500);
    }
  }


  // Create a new node with the data sent from the device.
  public function saveNodeData(Request $request)
  {
    $this->is_authorize($request);
    $data = json_decode($request->getContent(), TRUE);

    if (!isset($data['title'], $data['key'], $data['uid'], $data['status'])) {
      return new JsonResponse(['error' => 'Missing required data'], 400);
    }

    try {

      /*$queryKeyPerm = \Drupal::database()->query(
        "SELECT ufkp.field_key_permission_value AS key_perm, rfid.entity_id AS uid FROM user__field_rfid AS rfid
        LEFT JOIN user__field_key_permission AS ufkp ON ufkp.entity_id = rfid.entity_id
						WHERE rfid.field_rfid_value = '" . $data['rfid'] . "'
					"
      )->fetchField();*/

      $node = Node::create([
        'type' => 'data_logs', // Replace with your content type machine name
        'title' => $data['title'], // Or a dynamic title
        'field_key' => $data['key'], // Replace with your text field 1 machine name
        'field_remakrs' => $data['status'], // Replace with your text field 1 machine name
        'field_status' => $data['status'], // Replace with your text field 1 machine name
      ]);

      $node->setOwnerId($data['uid']); // Set the node's owner

      if (isset($data['image'])) {
        // Get the base64 encoded image from the request.
        $base64_image = $data['image'];

        $filteredData = substr($base64_image, strpos($base64_image, ",") + 1);

        // Decode the base64 image.
        $decoded_image = base64_decode($filteredData);

        // Generate a unique file name.
        $file_name = 'image_' . time() . '.jpeg';

        // Define the file destination.
        $file_destination = 'public://images/' . $file_name;

        $file = \Drupal::service('file.repository')->writeData($decoded_image, $file_destination);

        $node->field_camera_photo->setValue([
          'target_id' => $file->id(),
          'alt' => 'Photo camera shot',
          'title' => 'Photo camera shot',
        ]);
      }

      $node->save();

      return new JsonResponse(['message' => 'Node created successfully', 'nid' => $node->id()], 200);
    } catch (\Exception $e) {
      \Drupal::logger('my_module')->error('Error creating node: @error', ['@error' => $e->getMessage()]);
      return new JsonResponse(['error' => 'Error creating node: ' . $e->getMessage()], 500);
    }
  }
}
