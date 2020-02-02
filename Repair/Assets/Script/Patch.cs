using DefaultNamespace;
using UnityEngine;

namespace Script
{
    public class Patch: MonoBehaviour
    {

        private HoleShape Shape;
        private bool _isGrabbed;

        public bool isGrabbed()
        {
            return _isGrabbed;
        }

        public object ChangeShape()
        {
            this.Shape = (HoleShape)Random.Range(0, 2);
            return this.gameObject;
        }
    }
}