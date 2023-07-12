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
        transform.localRotation = Quaternion.Slerp(transform.localRotation, SerialController.self.outputRotation, Time.deltaTime * easing);
	}
}
