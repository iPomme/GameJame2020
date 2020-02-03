using System;
using UnityEngine;
using UnityEngine.InputSystem;

public class CameraMovement : MonoBehaviour
{
    public float speed = 550f;
    private Vector3 inputDirection;
    private Vector3 movment;
    
    PlayerInputActions inputAction;
    
    // Move
    Vector2 movementInput;
    // Look direction
    Vector2 lookPosition;

    private void Awake()
    {
        inputAction = new PlayerInputActions();
        inputAction.PlayerControls.Move.performed += ctx => movementInput = ctx.ReadValue<Vector2>();
        inputAction.PlayerControls.FireDirection.performed += ctx => lookPosition = ctx.ReadValue<Vector2>();
    }

    
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
 
    void Update()
    {
        
        Vector3 targetInput = new Vector3(lookPosition.x,0,lookPosition.y);
        inputDirection = Vector3.Lerp(inputDirection,targetInput,Time.deltaTime*speed);
        //transform.position = inputDirection;
        transform.position += inputDirection;
    }

    private void OnEnable()
    {
        inputAction.Enable();
    }
    private void OnDisable()
    {
        inputAction.Disable();
    }
}
