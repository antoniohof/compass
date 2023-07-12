using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ApplyRotation : MonoBehaviour {

    public float easing = 2.0f;

	// Use this for initialization
	void Start () {
	}

	// Update is called once per frame
	void Update () {
        Vector3 relativePos = Camera.main.transform.position - transform.position;

        // the second argument, upwards, defaults to Vector3.up
        Quaternion rotation = Quaternion.LookRotation(relativePos, Vector3.up);
        transform.localRotation = Quaternion.Slerp(transform.localRotation, SerialController.self.outputRotation * rotation, Time.deltaTime * easing);
	}
}
