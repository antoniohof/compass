using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ApplyRotation : MonoBehaviour {

    float easing = 5.0f;

	// Use this for initialization
	void Start () {
	}
	
	// Update is called once per frame
	void Update () {
        transform.rotation = Quaternion.Slerp(transform.rotation, SerialController.self.outputRotation, Time.deltaTime * easing);
	}
}
