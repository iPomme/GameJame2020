using DefaultNamespace;
using UnityEngine;

namespace Script
{
    public class Patch : MonoBehaviour
    {
        private HoleShape Shape;
        private bool _isGrabbed;
        private GameHandling _game;


        public void SetGameHandling(GameObject gameHandling)
        {
            this._game = gameHandling.GetComponent<GameHandling>();
        }

        public bool isGrabbed()
        {
            return _isGrabbed;
        }

        public object ChangeShape()
        {
            this.Shape = (HoleShape) Random.Range(0, 2);
            return this.gameObject;
        }

        private void OnTriggerEnter(Collider other)
        {
            Ecoutille ecoutille = other.gameObject.GetComponentInParent<Ecoutille>();
            Debug.LogFormat("OnTriggerEnter ({0}) tag ({1}) trigered ...", other.gameObject.name, other.gameObject.tag);
            if (other.gameObject.tag == "Shape1")
            {
                Debug.Log("DESTROY");
                _game.PatchMatched(ecoutille);
            }
        }

        void OnCollisionEnter(Collision collision)
        {
            Debug.LogFormat("Collider ({0}) tag ({1}) trigered ...", collision.gameObject.name,
                collision.gameObject.tag);
            if (collision.gameObject.tag == "Shape1") Debug.Log("Collision");
            // _game.PatchMatched();
        }
    }
}