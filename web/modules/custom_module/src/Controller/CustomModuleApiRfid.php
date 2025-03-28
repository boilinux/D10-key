<?php

declare(strict_types=1);

namespace Drupal\custom_module\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\HttpFoundation\JsonResponse;
use Symfony\Component\HttpFoundation\Request;
use Drupal\node\Entity\Node;
use Drupal\user\Entity\User;

/**
 * Returns responses for custom_module routes.
 */
final class CustomModuleApiRfid extends ControllerBase
{

  public function checkRfid(Request $request)
  {
    $jsonDataResponse = new \stdClass();

    $requestToken = $request->headers->get('Authorization');
    $adminUser = User::load(1);
    $restToken = $adminUser->field_rest_token->value;

    if ($requestToken != $restToken) {
      return new JsonResponse(['error' => 'Access Denied'], 400);
    }

    $data = json_decode($request->getContent(), TRUE);

    if (!isset($data['rfid'])) {
      return new JsonResponse(['error' => 'Missing required data'], 400);
    }

    try {
      $queryKeyPerm = \Drupal::database()->query(
        "SELECT ufkp.field_key_permission_value AS key_perm, rfid.entity_id AS uid FROM user__field_rfid AS rfid
        LEFT JOIN user__field_key_permission AS ufkp ON ufkp.entity_id = rfid.entity_id
						WHERE rfid.field_rfid_value = '" . $data['rfid'] . "'
					"
      )->fetchAll();

      if (!$queryKeyPerm) {
        return new JsonResponse(['error' => 'User not found'], 404);
      }
      $key_perm = [];
      $uid = 0;
      foreach ($queryKeyPerm as $data) {
        $key_perm[] = $data->key_perm;
        $uid = $data->uid;
      }

      $jsonResponse = new JsonResponse(['uid' => $uid, 'k' => $key_perm]);

      \Drupal::logger('custom_module')->info($jsonResponse);

      return $jsonResponse;
    } catch (\Exception $e) {
      \Drupal::logger('my_module')->error('Error checking RFID: @error', ['@error' => $e->getMessage()]);
      return new JsonResponse(['error' => 'Error checking RFID: ' . $e->getMessage()], 500);
    }
  }
}
