using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PaddleTracker : MonoBehaviour
{
    private Quaternion position;
    // Start is called before the first frame update
    void Start()
    {
        position = this.transform.localRotation;
    }

    
    // Update is called once per frame
    void Update()
    {
        this.transform.localRotation = position;
    }

    public void setNewPosition(Quaternion newPosition)
    {
        position = newPosition;
    }
}

