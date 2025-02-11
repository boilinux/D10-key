<?php

declare(strict_types=1);

namespace Drupal\custom_module\Controller;

use Drupal\Core\Controller\ControllerBase;
use Symfony\Component\HttpFoundation\JsonResponse;
use Symfony\Component\HttpFoundation\Request;
use Drupal\node\Entity\Node;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\user\Entity\User;
use Drupal\file\Entity\File;
use Drupal\media\Entity\Media;
use Drupal\Core\File\FileSystemInterface;


/**
 * Returns responses for custom_module routes.
 */
final class CustomModuleController extends ControllerBase
{
  /**
   * The entity type manager.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * MyModuleController constructor.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The entity type manager.
   */
  public function __construct(EntityTypeManagerInterface $entity_type_manager)
  {
    $this->entityTypeManager = $entity_type_manager;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container)
  {
    return new static(
      $container->get('entity_type.manager')
    );
  }

  public function saveNodeData(Request $request)
  {
    $requestToken = $request->headers->get('Authorization');
    $adminUser = User::load(1);
    $restToken = $adminUser->field_rest_token->value;

    if ($requestToken != $restToken) {
      return new JsonResponse(['error' => 'Access Denied'], 400);
    }

    $data = json_decode($request->getContent(), TRUE);

    if (!isset($data['title'], $data['status'], $data['image'])) {
      return new JsonResponse(['error' => 'Missing required data'], 400);
    }

    try {
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

      $node = Node::create([
        'type' => 'data_logs', // Replace with your content type machine name
        'title' => $data['title'], // Or a dynamic title
        'field_status' => $data['status'], // Replace with your text field 1 machine name
        'author' => 1,
      ]);
      $node->setOwnerId(1); // Set the node's owner

      $node->field_camera_photo->setValue([
        'target_id' => $file->id(),
        'alt' => 'Photo camera shot',
        'title' => 'Photo camera shot',
      ]);

      $node->save();

      return new JsonResponse(['message' => 'Node created successfully', 'nid' => $node->id()], 201);
    } catch (\Exception $e) {
      \Drupal::logger('my_module')->error('Error creating node: @error', ['@error' => $e->getMessage()]);
      return new JsonResponse(['error' => 'Error creating node: ' . $e->getMessage()], 500);
    }
  }
}
