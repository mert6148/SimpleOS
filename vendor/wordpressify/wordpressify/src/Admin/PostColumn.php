<?php

namespace WP\Admin;

use WP\Core\Hooks\Action;
use WP\Core\Hooks\Filter;

class PostColumn implements Action, Filter {

	public $id = null;

	public $name = null;

	public $types = null;

	/**
	 * @var null|\WP_Screen
	 */
	public $screen = null;

	public $content = null;

	public function __construct( $id, $name, Callable $content = null, $types = null ) {

		if ( $id && $name && is_callable($content) ) {
			$this->id   = $id;
			$this->name = $name;
			if ( is_string( $types ) ) {
				$this->types = [ $types ];
			}

			$this->content = $content;
			\add_action( 'load-edit.php', [ $this, 'load' ] );
		}

		return $this;
	}

	public function load() {
		if ( $this->types ) {
			if (
				! isset( $screen->post_type ) and ! in_array( $this->screen->post_type, $this->types )
			) {
				return;
			}
		}
		$this->screen  = \get_current_screen();
		if(is_a($this->screen,'WP_Screen')){
			$this->add_filters();
			$this->add_actions();
		}
	}

	public function add_filters() {
		\add_filter( "manage_{$this->screen->id}_columns", [ $this, 'cols' ], 10, 1 );
	}

	public function add_actions() {
		\add_action( "manage_{$this->screen->post_type}_posts_custom_column", [ $this, 'col_content' ], 10, 2 );
	}

	public function cols( $cols ) {
		$cols[ $this->id ] = $this->name;
		return $cols;
	}

	public function col_content( $col, $post_id ) {
		if ( $this->id == $col ) {
			$content = $this->content;
			$content( $post_id );
		}
	}
}