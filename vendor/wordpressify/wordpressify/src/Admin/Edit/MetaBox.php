<?php
namespace WP\Admin\Edit;

class MetaBox
{
	/**
	 * Save callback
	 * @var callable|null
	 */
	public $save = null;

	/**
	 * Render callback
	 * @var callable|null
	 */
	public $render = null;

	/**
	 * MetaBox constructor.
	 * @param string $id
	 * @param string $name
	 * @param array $screens
	 * @param callable|null $render
	 * @param callable|null $save
	 */
	public function __construct($id = '', $name = '', $screens = [], Callable $render = null, Callable $save = null)
	{
		if (is_admin()) {
			$this->id = $id;
			$this->name = $name;
			$this->screens = $screens;
			$this->render = $render;
			$this->save = $save;
			add_action('load-post.php', array($this, 'init_metabox'));
			add_action('load-post-new.php', array($this, 'init_metabox'));
		}

	}

	/**
	 * Meta box initialization.
	 */
	public function init_metabox()
	{
		add_action('add_meta_boxes', array($this, 'add_metabox'));
		add_action('save_post', array($this, 'save_metabox'), 10, 2);
	}

	/**
	 * Adds the meta box.
	 */
	public function add_metabox()
	{
		foreach ($this->screens as $screen) {
			add_meta_box(
				$this->id,
				$this->name,
				array($this, 'render_metabox'),
				$screen,
				'advanced',
				'default'
			);
		}

	}

	/**
	 * Renders the meta box.
	 */
	public function render_metabox(\WP_Post $post)
	{
		// Add nonce for security and authentication.
		wp_nonce_field('custom_nonce_action', 'custom_nonce');
		$render = $this->render;
		do_action("adviva/booking/admin/metas/$this->id/render/top", [$post]);
		if ($render) {
			echo '<div>';
			$render($post);
			echo '</div>';
		}
		do_action("adviva/booking/admin/metas/$this->id/render/bottom", [$post]);
	}

	/**
	 * Handles saving the meta box.
	 * @param int $post_id Post ID.
	 * @param \WP_Post $post Post object.
	 * @return null
	 */
	public function save_metabox($post_id, \WP_Post $post)
	{

		// Add nonce for security and authentication.
		$nonce_name = isset($_POST['custom_nonce']) ? $_POST['custom_nonce'] : '';
		$nonce_action = 'custom_nonce_action';

		// Check if nonce is valid.
		if (!wp_verify_nonce($nonce_name, $nonce_action)) {
			return;
		}

		// Check if user has permissions to save data.
		if (!current_user_can('edit_post', $post_id)) {
			return;
		}

		// Check if not an autosave.
		if (wp_is_post_autosave($post_id)) {
			return;
		}

		// Check if not a revision.
		if (wp_is_post_revision($post_id)) {
			return;
		}

		// All fine - saving method
		$save = $this->save;
		if ($save) {
			$save($post);
		}
		return;
	}
}
