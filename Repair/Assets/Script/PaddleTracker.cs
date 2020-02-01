using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PaddleTracker : MonoBehaviour
{
    public float rotationSpeed = 100f;

    private Quaternion reference;
    private Quaternion rotation;
    // Start is called before the first frame update
    void Start()
    {
        reference = transform.rotation;
        rotation = this.transform.localRotation;
    }

    
    // Update is called once per frame
    void Update()
    {
        // this.gameObject.transform.localRotation =  Quaternion.Slerp(this.gameObject.transform.localRotation,rotation,Time.deltaTime * rotationSpeed);
        this.gameObject.transform.localRotation = rotation;

    }

    public void setNewPosition(Quaternion newPosition)
    {
        // Quaternion rotTmp = Quaternion.LookRotation(newPosition.ToEuler(), Vector3.down);
        //
        // rotTmp.x = newPosition.w;
        // newPosition *= Quaternion.Euler(Vector3.up * -newPosition.eulerAngles.y);
        rotation = newPosition;// * Quaternion.Inverse(reference);
        // this.gameObject.transform.localRotation = rotation;
    }

    public void setjoystick(ushort jx, ushort jy, ushort jz)
    {
        if (jz == 1)
        {
            OVRInput.SetControllerVibration(1, 1, OVRInput.Controller.RTouch);
        }
        else
        {
            OVRInput.SetControllerVibration(0, 0, OVRInput.Controller.RTouch);
        }
    }
}

