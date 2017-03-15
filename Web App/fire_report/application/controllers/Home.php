<?php
defined('BASEPATH') OR exit('No direct script access allowed');

class Home extends CI_Controller {

	public function index()
	{
		double fire_lat;
		double fire_long;


		$this->load->library('googlemaps');

		$config['center'] = 'lapulapu, philippines';
		$config['zoom'] = 'auto';
		$this->googlemaps->initialize($config);

		$marker = array();
		$marker['position'] = '10.3098029, 123.9478881';
		$marker['infowindow_content'] = 'Poblacion';
		//$marker['icon'] = 'assets/image/fire_alert.png';
		$this->googlemaps->add_marker($marker);

		$marker = array();
		$marker['position'] = '10.3046919, 123.9570296';
		$marker['infowindow_content'] = 'Hoops Dome';
		$this->googlemaps->add_marker($marker);

		$marker = array();
		$marker['position'] = '10.2854182, 123.9446834';
		$marker['infowindow_content'] = 'Cordova';
		$this->googlemaps->add_marker($marker);

		$marker = array();
		$marker['position'] = '10.3090222, 124.0109948';
		$marker['infowindow_content'] = 'Pusok';
		//$marker['draggable'] = TRUE;
		//$marker['animation'] = 'DROP';
		//$marker['onclick'] = 'alert("You just clicked me!!")';
		$this->googlemaps->add_marker($marker);

		/*for line
		$polyline = array();
		$polyline['points'] = array('10.3090222, 124.0109948',
			    '10.2854182, 123.9446834');
		$this->googlemaps->add_polyline($polyline);*/
		/*
		$config['directions'] = TRUE;
		$config['directionsStart'] = '10.3046919, 123.9570296';
		$config['directionsEnd'] = '10.3098029, 123.9478881';
		$config['directionsDivID'] = 'directionsDiv';
		$this->googlemaps->initialize($config);
		*/
		$data['map'] = $this->googlemaps->create_map();

		$this->load->view('Maps', $data);
	}
}
