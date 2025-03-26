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
use stdClass;

/**
 * Returns responses for custom_module routes.
 */
final class CustomModuleControllerSms extends ControllerBase
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

  public function getSmsNotification(Request $request)
  {
    $this->is_authorize($request);

    $nodes = [];
    $phone_numbers = [];

    $queryMessage = \Drupal::database()->query(
      "SELECT nfd.nid,nfd.title FROM node_field_data AS nfd
      LEFT JOIN node__field_message_status AS nfms ON nfms.entity_id = nfd.nid
      WHERE nfd.type = 'notification_logs' AND nfms.field_message_status_value = 0"
    )->fetchAll();

    $counter = 0;
    foreach ($queryMessage as $record) {
      $nodes[$counter]['nid'] = $record->nid;
      $nodes[$counter]['title'] = $record->title;

      $counter++;
    }

    $queryPhoneNumbers = \Drupal::database()->query(
      "SELECT nfpn.field_phone_number_value AS phone_number FROM node_field_data AS nfd
      LEFT JOIN node__field_phone_number AS nfpn ON nfpn.entity_id = nfd.nid
      WHERE nfd.type = 'mobile_number'"
    )->fetchAll();

    foreach ($queryPhoneNumbers as $record) {
      $phone_numbers[] = $record->phone_number;
    }

    return new JsonResponse(['message' => 'notification logs', 'nodes' => $nodes, 'phone_numbers' => $phone_numbers], 200);
  }

  public function postSmsNotification(Request $request)
  {
    $this->is_authorize($request);

    $data = json_decode($request->getContent(), TRUE);

    if (!isset($data['nid'])) {
      return new JsonResponse(['error' => 'Missing required data'], 400);
    }
    //
    foreach ($data['nid'] as $nid) {
      $node = Node::load($nid);
      $node->field_message_status->setValue([
        'value' => 1,
      ]);

      $node->save();
    }
    return new JsonResponse(['message' => 'update notification logs', 'nodes' => $data['nid']], 200);
  }
}
