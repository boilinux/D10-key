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
    $data['what_image'] = $request->request->get('what_image');
    $data['image'] = $request->files->get('image');

    \Drupal::logger('my_module')->error('nid: @nid, what_image: @what_image', ['@nid' => $request->request->get('nid'), '@what_image' => $data['what_image']]);

    if (!isset($data['what_image']) || !isset($data['image']) || !isset($data['nid'])) {
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

          $what_image = $data['what_image'];

          // save to camera photo
          if ($what_image == 'front') {
            $node->field_camera_photo->setValue([
              'target_id' => $file_entity->id(),
              'alt' => 'Photo camera shot',
              'title' => 'Photo camera shot',
            ]);
          }
          // save to camera photo 2
          else if ($what_image == 'back') {
            $node->field_camera_photo2->setValue([
              'target_id' => $file_entity->id(),
              'alt' => 'Photo camera shot 2',
              'title' => 'Photo camera shot 2',
            ]);
          }


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
      $myKey = $data['key'];
      // assume no record then its borrowed
      $ir = $data['status'];
      $status = "success";
      $remarks = "borrowed";

      // assume borrowed if ir yes_detection, then failed
      if ($ir == "yes_detection") {
        $status = "failed";
        $remarks = "borrowed";
      } else if ($ir == "no_detection") {
        $status = "success";
        $remarks = "borrowed";
      }


      $queryRecord = \Drupal::database()->query(
        "SELECT nfd.nid,nfs.field_status_value AS status, nfr.field_remarks_value AS remarks, nfk.field_key_value AS key_value
        FROM node_field_data AS nfd
        LEFT JOIN node__field_key AS nfk ON nfk.entity_id = nfd.nid
        LEFT JOIN node__field_status AS nfs ON nfs.entity_id = nfd.nid
        LEFT JOIN node__field_remarks AS nfr ON nfr.entity_id = nfd.nid
        WHERE nfd.type='data_logs' AND nfk.field_key_value = " . $data['key'] . " ORDER BY nfd.nid DESC"
      )->fetchAssoc();

      // if query has record
      if ($queryRecord) {
        $myKey = $queryRecord['key_value'];
        $status = $queryRecord['status'];
        $remarks = $queryRecord['remarks'];

        if ($remarks == "returned") {
          if ($status == "failed") {
            if ($ir == "yes_detection") {
              $status = "failed";
              $remarks = "returned";
            } else if ($ir == "no_detection") {
              $status = "success";
              $remarks = "borrowed";
            }
          } else {
            if ($ir == "no_detection") {
              $status = "failed";
              $remarks = "returned";
            } else if ($ir == "yes_detection") {
              $status = "success";
              $remarks = "borrowed";
            }
          }
        }
        // if borrowed
        if ($remarks == "borrowed") {
          if ($status == "failed") {
            if ($ir == "no_detection") {
              $status = "success";
              $remarks = "borrowed";
            } else if ($ir == "yes_detection") {
              $status = "failed";
              $remarks = "borrowed";
            }
          } else {
            if ($ir == "no_detection") {
              $status = "failed";
              $remarks = "borrowed";
            } else if ($ir == "yes_detection") {
              $status = "success";
              $remarks = "returned";
            }
          }
        }
      }

      $strTitle = "Key " . $myKey . " is " . $remarks;

      // create a data_logs node
      $node = Node::create([
        'type' => 'data_logs', // Replace with your content type machine name
        'title' => $strTitle, // Or a dynamic title
        'field_key' => $myKey, // Replace with your text field 1 machine name
        'field_remarks' => $remarks, // Replace with your text field 1 machine name
        'field_status' => $status, // Replace with your text field 1 machine name
      ]);

      $node->setOwnerId($data['uid']); // Set the node's owner

      $node->save();

      // create a notification_logs node
      $user = User::load($data['uid']);
      $strNotificationTitle = "Key " . $myKey . " is " . $remarks . " by " . $user->getDisplayName() . " at " . date('Y-m-d H:i:s') . " Status: " . $status;
      $nodeNotification = Node::create([
        'type' => 'notification_logs', // Replace with your content type machine name
        'title' => $strNotificationTitle, // Or a dynamic title
      ]);
      $nodeNotification->setOwnerId($data['uid']); // Set the node's owner
      $nodeNotification->save();

      return new JsonResponse(['message' => 'Node created successfully', 'nid' => $node->id()], 200);
    } catch (\Exception $e) {
      \Drupal::logger('my_module')->error('Error creating node: @error', ['@error' => $e->getMessage()]);
      return new JsonResponse(['error' => 'Error creating node: ' . $e->getMessage()], 500);
    }
  }
}
