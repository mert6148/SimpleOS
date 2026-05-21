<?php

function posts($args=[])
{
    return (new \WP\Posts\Types\PostType('post'))->posts($args);
}

function pages($args=[])
{
    return (new \WP\Posts\Types\PostType('page'))->posts($args);
}

function wordpress(){
	return \WP\WP::getInstance();
}