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

    if (!isset($data['title'], $data['status'])) {
      return new JsonResponse(['error' => 'Missing required data'], 400);
    }

    try {
      $node = Node::create([
        'type' => 'data_logs', // Replace with your content type machine name
        'title' => $data['title'], // Or a dynamic title
        'field_status' => $data['status'], // Replace with your text field 1 machine name
        'author' => 1,
      ]);
      $node->setOwnerId(1); // Set the node's owner

      $node->save();

      return new JsonResponse(['message' => 'Node created successfully', 'nid' => $node->id()], 201);
    } catch (\Exception $e) {
      \Drupal::logger('my_module')->error('Error creating node: @error', ['@error' => $e->getMessage()]);
      return new JsonResponse(['error' => 'Error creating node: ' . $e->getMessage()], 500);
    }
  }
}
