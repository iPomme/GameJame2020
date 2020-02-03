using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class CameraMovement : MonoBehaviour
{
    
    PlayerInputActions inputAction;
    
    // Move
    Vector2 movementInput;
    // Look direction
    Vector2 lookPosition;

    private void Awake()
    {
        CreatePlayerMovementPlane();
        inputAction = new PlayerInputActions();
        inputAction.PlayerControls.Move.performed += ctx 
    }

    void CreatePlayerMovementPlane()
    {
        playerMovementPlane = new Plane(transform.up,transform.position+transform.up);
    }
    
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    public float speed = 5.0f;
    void Update()
    {
        if (Input.GetKey(KeyCode.RightArrow))
        {
            transform.position = new Vector3(speed * Time.deltaTime, 0, 0);
        }
        if (Input.GetKey(KeyCode.LeftArrow))
        {
            transform.position = new Vector3(speed * Time.deltaTime, 0, 0);
        }
        if (Input.GetKey(KeyCode.DownArrow))
        {
            transform.position = new Vector3(0, speed * Time.deltaTime, 0);
        }
        if (Input.GetKey(KeyCode.UpArrow))
        {
            transform.position = new Vector3(0, speed * Time.deltaTime, 0);
        }
    
    }
}
